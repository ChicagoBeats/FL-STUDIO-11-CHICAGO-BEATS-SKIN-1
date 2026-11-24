#pragma once
#include <JuceHeader.h>

class ChicagoBeatsBloodlineAudioProcessor final : public juce::AudioProcessor {
public:
    ChicagoBeatsBloodlineAudioProcessor();
    ~ChicagoBeatsBloodlineAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    float getLow() const noexcept { return low.load(); }
    float getMid() const noexcept { return mid.load(); }
    float getHigh() const noexcept { return high.load(); }
    float getPeak() const noexcept { return peak.load(); }
    float getKick() const noexcept { return kick.load(); }
    int getLastMidiNote() const noexcept { return lastMidiNote.load(); }
    float getMidiPulse() const noexcept { return midiPulse.load(); }

private:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;
    juce::dsp::FFT fft { fftOrder };
    std::array<float, fftSize> fifo {};
    std::array<float, fftSize * 2> fftData {};
    int fifoIndex = 0;
    std::atomic<float> low { 0 }, mid { 0 }, high { 0 }, peak { 0 }, kick { 0 }, midiPulse { 0 };
    std::atomic<int> lastMidiNote { 60 };
    float previousLow = 0.0f;
    double currentSampleRate = 44100.0;
    void pushNextSampleIntoFifo(float sample) noexcept;
    void performFFT() noexcept;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChicagoBeatsBloodlineAudioProcessor)
};
