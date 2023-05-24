/*
  ==============================================================================

    StyleSheet.h
    Created: 13 May 2023 4:17:12pm
    Author:  leanne

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
// class StyleSheet  : public juce::Component
// {
// public:
//     StyleSheet();
//     ~StyleSheet() override;

//     void paint (juce::Graphics&) override;
//     void resized() override;

// private:
//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StyleSheet)
// };

namespace juce {


class CustomLNF : public juce::LookAndFeel_V4
{
  public:
    CustomLNF(); // [1]
    // ~CustomLNF() override;
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;

  private:
    // CustomLNF customLNF; // [2]
    // juce::Slider dial1;
    // juce::Slider dial2;
    // juce::TextButton button1;
    // juce::TextButton button2;

};
}
