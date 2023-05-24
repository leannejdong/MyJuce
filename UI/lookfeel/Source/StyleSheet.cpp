/*
  ==============================================================================

    StyleSheet.cpp
    Created: 13 May 2023 4:17:12pm
    Author:  leanne

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StyleSheet.h"

namespace juce
{
  CustomLNF::CustomLNF()
  {
    // setColour(Slider::rotarySliderOutlineColorId, Colours::red);
    // setColour(Slider::rotarySliderFillColourId, Colours::green);
    setColour(Slider::thumbColourId, Colours::purple);
  }
  void CustomLNF::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)              
  {
        auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));

    // auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    //     auto centreX = (float) x + (float) width  * 0.5f;
    //     auto centreY = (float) y + (float) height * 0.5f;
    //     auto rx = centreX - radius;
    //     auto ry = centreY - radius;
    //     auto rw = radius * 2.0f;
    //     auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    //     // fill
    //     g.setColour (juce::Colours::orange);
    //     g.fillEllipse (rx, ry, rw, rw);

    //     // outline
    //     g.setColour (juce::Colours::red);
    //     g.drawEllipse (rx, ry, rw, rw, 1.0f);

    //     juce::Path p;
    //     auto pointerLength = radius * 0.33f;
    //     auto pointerThickness = 2.0f;
    //     p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    //     p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

    //     // pointer
    //     g.setColour (juce::Colours::yellow);
    //     g.fillPath (p);

  }
}