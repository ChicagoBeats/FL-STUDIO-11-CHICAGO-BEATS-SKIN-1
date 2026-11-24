#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ChicagoBeatsBloodlineAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                       private juce::Timer {
public:
    explicit ChicagoBeatsBloodlineAudioProcessorEditor(ChicagoBeatsBloodlineAudioProcessor&);
    ~ChicagoBeatsBloodlineAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    void drawHeader(juce::Graphics&, juce::Rectangle<float>);
    void drawCarRunway(juce::Graphics&, juce::Rectangle<float>);
    void drawPiano(juce::Graphics&, juce::Rectangle<float>);
    void drawSpeakers(juce::Graphics&, juce::Rectangle<float>);
    ChicagoBeatsBloodlineAudioProcessor& processor;
    juce::Image carImage, speakerImage;
    juce::TextButton cruiseButton { "BACK & FORTH" }, burnoutButton { "BURNOUT" }, donutButton { "DONUTS" }, holdButton { "HOLD" };
    enum class CarMode { cruise, burnout, donut, hold };
    CarMode carMode = CarMode::cruise;
    float carPosition = 0.12f, carDirection = 1.0f, carRotation = 0.0f, visualTime = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChicagoBeatsBloodlineAudioProcessorEditor)
};
