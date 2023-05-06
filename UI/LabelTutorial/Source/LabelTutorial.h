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

 name:             LabelTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays labels.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2019, linux_make, xcode_iphone, androidstudio

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class MainContentComponent   : public juce::Component
{
public:
    //==============================================================================
    MainContentComponent()
    {
        addAndMakeVisible (titleLabel);
        titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
        titleLabel.setText ("Click in the white box to enter some text...", juce::dontSendNotification);
        titleLabel.setColour (juce::Label::textColourId, juce::Colours::lightgreen);
        titleLabel.setJustificationType (juce::Justification::centred);

        addAndMakeVisible (inputLabel);
        inputLabel.setText ("Text input:", juce::dontSendNotification);
        inputLabel.attachToComponent (&inputText, true);
        inputLabel.setColour (juce::Label::textColourId, juce::Colours::orange);
        inputLabel.setJustificationType (juce::Justification::right);

        addAndMakeVisible (uppercaseLabel);
        uppercaseLabel.setText ("Uppercase:", juce::dontSendNotification);
        uppercaseLabel.attachToComponent (&uppercaseText, true);
        uppercaseLabel.setColour (juce::Label::textColourId, juce::Colours::orange);
        uppercaseLabel.setJustificationType (juce::Justification::right);

        addAndMakeVisible (uppercaseText);
        uppercaseText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);

        addAndMakeVisible (inputText);
        inputText.setEditable (true);
        inputText.setColour (juce::Label::backgroundColourId, juce::Colours::darkblue);
        inputText.onTextChange = [this] { uppercaseText.setText (inputText.getText().toUpperCase(), juce::dontSendNotification); };

        setSize (320, 200);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
    }

    void resized() override
    {
        titleLabel   .setBounds (10,  10, getWidth() - 20,  30);
        inputText    .setBounds (100, 50, getWidth() - 110, 20);
        uppercaseText.setBounds (100, 80, getWidth() - 110, 20);
    }

private:
    //==============================================================================
    juce::Label titleLabel;
    juce::Label inputLabel;
    juce::Label inputText;
    juce::Label uppercaseLabel;
    juce::Label uppercaseText;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
