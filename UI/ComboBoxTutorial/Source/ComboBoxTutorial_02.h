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

 name:             ComboBoxTutorial
 version:          2.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays combo boxes.

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
    enum FontStyles
    {
        stylePlain = 1,
        styleBold,
        styleItalic,
        numberOfStyles
    };

    enum TextColours
    {
        black = 1,
        white,
        red,
        darkred,
        indianred,
        green,
        darkgreen,
        lightgreen,
        blue,
        darkblue,
        lightblue,
        numberOfColours
    };

    //==============================================================================
    MainContentComponent()
    {
        addAndMakeVisible (textLabel);
        textLabel.setFont (textFont);

        addAndMakeVisible (styleMenu);

        // add items to the combo-box
        styleMenu.addItem ("Plain",  1);
        styleMenu.addItem ("Bold",   2);
        styleMenu.addItem ("Italic", 3);

        styleMenu.onChange = [this] { styleMenuChanged(); };
        styleMenu.setSelectedId (stylePlain);

        addAndMakeVisible (coloursMenu);

        coloursMenu.addItem ("Black", black);
        coloursMenu.addItem ("White", white);
        coloursMenu.addSeparator();

        coloursMenu.addSectionHeading ("Reds");
        coloursMenu.addItem ("Red",        red);
        coloursMenu.addItem ("Dark Red",   darkred);
        coloursMenu.addItem ("Indian Red", indianred);
        coloursMenu.addSeparator();

        coloursMenu.addSectionHeading ("Greens");
        coloursMenu.addItem ("Green",       green);
        coloursMenu.addItem ("Dark Green",  darkgreen);
        coloursMenu.addItem ("Light Green", lightgreen);
        coloursMenu.addSeparator();

        coloursMenu.addSectionHeading ("Blues");
        coloursMenu.addItem ("Blue",       blue);
        coloursMenu.addItem ("Dark Blue",  darkblue);
        coloursMenu.addItem ("Light Blue", lightblue);

        coloursMenu.onChange = [this] { coloursMenuChanged(); };
        coloursMenu.setSelectedId (black);

        coloursMenu.setEditableText (true);

        setSize (400, 200);
    }

    void resized() override
    {
        textLabel  .setBounds (10, 10, getWidth() - 20, 20);
        styleMenu  .setBounds (10, 40, getWidth() - 20, 20);
        coloursMenu.setBounds (10, 70, getWidth() - 20, 20);
    }

private:
    void styleMenuChanged()
    {
        switch (styleMenu.getSelectedId())
        {
            case stylePlain:  setStylePlain();  break;
            case styleBold:   setStyleBold();   break;
            case styleItalic: setStyleItalic(); break;
        }

        textLabel.setFont (textFont);
    }

    void setStylePlain()
    {
        setLightColoursEnabled (true);
        textFont.setStyleFlags (juce::Font::plain);
    }

    void setStyleBold()
    {
        setLightColoursEnabled (false);
        textFont.setStyleFlags (juce::Font::bold);
    }

    void setStyleItalic()
    {
        setLightColoursEnabled (true);
        textFont.setStyleFlags (juce::Font::italic);
    }

    void coloursMenuChanged()
    {
        juce::Colour textColour;

        switch (coloursMenu.getSelectedId())
        {
            case black:      textColour = juce::Colours::black;      break;
            case white:      textColour = juce::Colours::white;      break;
            case red:        textColour = juce::Colours::red;        break;
            case darkred:    textColour = juce::Colours::darkred;    break;
            case indianred:  textColour = juce::Colours::indianred;  break;
            case green:      textColour = juce::Colours::green;      break;
            case darkgreen:  textColour = juce::Colours::darkgreen;  break;
            case lightgreen: textColour = juce::Colours::lightgreen; break;
            case blue:       textColour = juce::Colours::blue;       break;
            case darkblue:   textColour = juce::Colours::darkblue;   break;
            case lightblue:  textColour = juce::Colours::lightblue;  break;
            default:         textColour = handleColourText();  break;
        }

        textLabel.setColour (juce::Label::textColourId, textColour);
    }

    juce::Colour handleColourText()
    {
        auto menuText = coloursMenu.getText();
        auto valueFromHex = (juce::uint32) menuText.getHexValue32();

        coloursMenu.setText (juce::String::toHexString ((int) valueFromHex));

        return juce::Colour (valueFromHex);
    }

    void setLightColoursEnabled (const bool shouldBeEnabled)
    {
        coloursMenu.setItemEnabled (white,      shouldBeEnabled);
        coloursMenu.setItemEnabled (indianred,  shouldBeEnabled);
        coloursMenu.setItemEnabled (lightgreen, shouldBeEnabled);
        coloursMenu.setItemEnabled (lightblue,  shouldBeEnabled);
    }

    //==============================================================================
    juce::Label textLabel { {}, "The quick brown fox jumps over the lazy dog." };
    juce::Font textFont   { 12.0f };
    juce::ComboBox styleMenu;
    juce::ComboBox coloursMenu;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
