/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AnalyticsCollectionTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Collects analytics data.

 dependencies:     juce_analytics, juce_core, juce_data_structures, juce_events,
                   juce_graphics, juce_gui_basics
 exporters:        xcode_mac, vs2019, linux_make, xcode_iphone, androidstudio

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

enum DemoAnalyticsEventTypes
{
    event,
    sessionStart,
    sessionEnd,
    screenView,
    exception
};

//==============================================================================
class GoogleAnalyticsDestination  : public juce::ThreadedAnalyticsDestination
{
public:
    GoogleAnalyticsDestination()
        : ThreadedAnalyticsDestination ("GoogleAnalyticsThread")
    {
        {
            // Choose where to save any unsent events.

            auto appDataDir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                                   .getChildFile (juce::JUCEApplication::getInstance()->getApplicationName());  // [1]

            if (! appDataDir.exists())
                appDataDir.createDirectory();                                                                   // [2]

            savedEventsFile = appDataDir.getChildFile ("analytics_events.xml");                                 // [3]
        }

        {
            // It's often a good idea to construct any analytics service API keys
            // at runtime, so they're not searchable in the binary distribution of
            // your application (but we've not done this here). You should replace
            // the following key with your own to get this example application
            // fully working.

            apiKey = "UA-XXXXXXXXX-1";
        }

        startAnalyticsThread (initialPeriodMs);                                                                 // [4]
    }

    ~GoogleAnalyticsDestination() override
    {
        // Here we sleep so that our background thread has a chance to send the
        // last lot of batched events. Be careful - if your app takes too long to
        // shut down then some operating systems will kill it forcibly!
        juce::Thread::sleep (initialPeriodMs);  // [5]

        stopAnalyticsThread (1000);             // [6]
    }

    int getMaximumBatchSize() override   { return 20; }

    bool logBatchedEvents (const juce::Array<AnalyticsEvent>& events) override
    {
        // Send events to Google Analytics.

        juce::String appData ("v=1&aip=1&tid=" + apiKey);       // [1]

        juce::StringArray postData;

        for (auto& event : events)                              // [2]
        {
            juce::StringPairArray data;

            switch (event.eventType)
            {
                case (DemoAnalyticsEventTypes::event):
                {
                    data.set ("t", "event");

                    if (event.name == "startup")
                    {
                        data.set ("ec",  "info");
                        data.set ("ea",  "appStarted");
                    }
                    else if (event.name == "shutdown")
                    {
                        data.set ("ec",  "info");
                        data.set ("ea",  "appStopped");
                    }
                    else if (event.name == "button_press")
                    {
                        data.set ("ec",  "button_press");
                        data.set ("ea",  event.parameters["id"]);
                    }
                    else if (event.name == "crash")
                    {
                        data.set ("ec",  "crash");
                        data.set ("ea",  "crash");
                    }
                    else
                    {
                        jassertfalse;
                        continue;
                    }

                    break;
                }

                default:
                {
                    // Unknown event type! In this demo app we're just using a
                    // single event type, but in a real app you probably want to
                    // handle multiple ones.
                    jassertfalse;
                    break;
                }
            }

            data.set ("cid", event.userID);                                 // [3]

            juce::StringArray eventData;

            for (auto& key : data.getAllKeys())                             // [4]
                eventData.add (key + "=" + juce::URL::addEscapeChars (data[key], true));

            postData.add (appData + "&" + eventData.joinIntoString ("&"));  // [5]
        }

        auto url = juce::URL ("https://www.google-analytics.com/batch")
                       .withPOSTData (postData.joinIntoString ("\n"));      // [6]

        {
            const juce::ScopedLock lock (webStreamCreation);        // [1]

            if (shouldExit)                                         // [2]
                return false;

            webStream.reset (new juce::WebInputStream (url, true)); // [3]
        }

        auto success = webStream->connect (nullptr);                // [4]

        // Do an exponential backoff if we failed to connect.
        if (success)
            periodMs = initialPeriodMs;
        else
            periodMs *= 2;

        setBatchPeriod (periodMs);                                  // [5]

        return success;
    }

    void stopLoggingEvents() override
    {
        const juce::ScopedLock lock (webStreamCreation);            // [1]

        shouldExit = true;                                          // [2]

        if (webStream.get() != nullptr)                             // [3]
            webStream->cancel();
    }

private:
    void saveUnloggedEvents (const std::deque<AnalyticsEvent>& eventsToSave) override
    {
        // Save unsent events to disk. Here we use XML as a serialisation format, but
        // you can use anything else as long as the restoreUnloggedEvents method can
        // restore events from disk. If you're saving very large numbers of events then
        // a binary format may be more suitable if it is faster - remember that this
        // method is called on app shutdown so it needs to complete quickly!

        juce::XmlDocument previouslySavedEvents (savedEventsFile);
        std::unique_ptr<juce::XmlElement> xml (previouslySavedEvents.getDocumentElement()); // [1]

        if (xml.get() == nullptr || xml->getTagName() != "events")                          // [2]
            xml.reset (new juce::XmlElement ("events"));

        for (auto& event : eventsToSave)
        {
            auto* xmlEvent = new juce::XmlElement ("google_analytics_event");               // [3]
            xmlEvent->setAttribute ("name", event.name);
            xmlEvent->setAttribute ("type", event.eventType);
            xmlEvent->setAttribute ("timestamp", (int) event.timestamp);
            xmlEvent->setAttribute ("user_id", event.userID);

            auto* parameters = new juce::XmlElement ("parameters");                         // [4]

            for (auto& key : event.parameters.getAllKeys())
                parameters->setAttribute (key, event.parameters[key]);

            xmlEvent->addChildElement (parameters);

            auto* userProperties = new juce::XmlElement ("user_properties");                // [5]

            for (auto& key : event.userProperties.getAllKeys())
                userProperties->setAttribute (key, event.userProperties[key]);

            xmlEvent->addChildElement (userProperties);

            xml->addChildElement (xmlEvent);                                                // [6]
        }

        xml->writeTo (savedEventsFile);                                                     // [7]
    }

    void restoreUnloggedEvents (std::deque<AnalyticsEvent>& restoredEventQueue) override
    {
        juce::XmlDocument savedEvents (savedEventsFile);
        std::unique_ptr<juce::XmlElement> xml (savedEvents.getDocumentElement());           // [1]

        if (xml.get() == nullptr || xml->getTagName() != "events")                          // [2]
            return;

        auto numEvents = xml->getNumChildElements();

        for (auto iEvent = 0; iEvent < numEvents; ++iEvent)
        {
            auto* xmlEvent = xml->getChildElement (iEvent);                                 // [3]

            juce::StringPairArray parameters;
            auto* xmlParameters = xmlEvent->getChildByName ("parameters");                  // [4]
            auto numParameters = xmlParameters->getNumAttributes();

            for (auto iParam = 0; iParam < numParameters; ++iParam)
                parameters.set (xmlParameters->getAttributeName (iParam),
                                xmlParameters->getAttributeValue (iParam));

            juce::StringPairArray userProperties;
            auto* xmlUserProperties = xmlEvent->getChildByName ("user_properties");         // [5]
            auto numUserProperties = xmlUserProperties->getNumAttributes();

            for (auto iProp = 0; iProp < numUserProperties; ++iProp)
                userProperties.set (xmlUserProperties->getAttributeName (iProp),
                                    xmlUserProperties->getAttributeValue (iProp));

            restoredEventQueue.push_back ({
                xmlEvent->getStringAttribute ("name"),                                      // [6]
                xmlEvent->getIntAttribute ("type"),
                static_cast<juce::uint32> (xmlEvent->getIntAttribute ("timestamp")),
                parameters,
                xmlEvent->getStringAttribute ("user_id"),
                userProperties
            });
        }

        savedEventsFile.deleteFile();                                                       // [7]
    }

    const int initialPeriodMs = 1000;
    int periodMs = initialPeriodMs;

    juce::CriticalSection webStreamCreation;
    bool shouldExit = false;
    std::unique_ptr<juce::WebInputStream> webStream;

    juce::String apiKey;

    juce::File savedEventsFile;
};

//==============================================================================
class MainContentComponent   : public juce::Component
{
public:
    //==============================================================================
    MainContentComponent()
    {
        // Add an analytics identifier for the user. Make sure you don't accidentally
        // collect identifiable information if you haven't asked for permission!
        juce::Analytics::getInstance()->setUserId ("AnonUser1234");                                 // [1]

        // Add any other constant user information.
        juce::StringPairArray userData;
        userData.set ("group", "beta");
        juce::Analytics::getInstance()->setUserProperties (userData);                               // [2]

        // Add any analytics destinations we want to use to the Analytics singleton.
        juce::Analytics::getInstance()->addDestination (new GoogleAnalyticsDestination());          // [3]

        // The event type here should probably be DemoAnalyticsEventTypes::sessionStart
        // in a more advanced app.
        juce::Analytics::getInstance()->logEvent ("startup", {}, DemoAnalyticsEventTypes::event);   // [4]

        crashButton.onClick = [this] { sendCrash(); };

        addAndMakeVisible (eventButton);
        addAndMakeVisible (crashButton);

        setSize (300, 200);

        juce::StringPairArray logButtonPressParameters;
        logButtonPressParameters.set ("id", "a");
        logEventButtonPress.reset (new juce::ButtonTracker (eventButton, "button_press", logButtonPressParameters)); // [2]
    }

    ~MainContentComponent() override
    {
        // The event type here should probably be DemoAnalyticsEventTypes::sessionEnd
        // in a more advanced app.
        juce::Analytics::getInstance()->logEvent ("shutdown", {}, DemoAnalyticsEventTypes::event);  // [5]
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        eventButton.centreWithSize (100, 40);
        eventButton.setBounds (eventButton.getBounds().translated (0, 25));
        crashButton.setBounds (eventButton.getBounds().translated (0, -50));
    }

private:
    //==============================================================================
    void sendCrash()
    {
        // In a more advanced application you would probably use a different event
        // type here.
        juce::Analytics::getInstance()->logEvent ("crash", {}, DemoAnalyticsEventTypes::event);
        juce::Analytics::getInstance()->getDestinations().clear();
        juce::JUCEApplication::getInstance()->quit();
    }

    juce::TextButton eventButton { "Press me!" }, crashButton { "Simulate crash!" };
    std::unique_ptr<juce::ButtonTracker> logEventButtonPress;   // [1]

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
