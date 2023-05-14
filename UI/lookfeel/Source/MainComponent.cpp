#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel (&customLNF);
    dial1.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dial1.setRange(0.0f, 100.0f);
    dial1.setValue(25.0f);
    dial1.setTextBoxStyle(juce::Slider::NoTextBox, true, 100, 25);
    addAndMakeVisible(dial1);

    button1.setButtonText ("Button 1");

    addAndMakeVisible (button1);

    setSize (300, 200);

    dial2.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    dial2.setRange(0.0f, 100.0f);
    dial2.setValue(25.0f);
    dial2.setTextBoxStyle(juce::Slider::NoTextBox, true, 100, 25);
    addAndMakeVisible(dial2);

    button2.setButtonText ("Button 2");

    addAndMakeVisible (button2);
    setSize (300, 200);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::gold);
    getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::antiquewhite);
      

    customLNF.setColour(juce::Slider::thumbColourId, juce::Colours::purple);
}

MainComponent::~MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::grey);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
   // dial1.setBounds(getWidth()/2 - 100, getHeight()/2 - 100, 200, 200);
    auto border = 4;

    auto area = getLocalBounds();

    auto dialArea = area.removeFromTop (area.getHeight() / 2);
    dial1.setBounds (dialArea.removeFromLeft (dialArea.getWidth() / 2).reduced (border));
    dial2.setBounds (dialArea.reduced (border));

    auto buttonHeight = 30;

    button1.setBounds (area.removeFromTop (buttonHeight).reduced (border));
    button2.setBounds (area.removeFromTop (buttonHeight).reduced (border));
}
