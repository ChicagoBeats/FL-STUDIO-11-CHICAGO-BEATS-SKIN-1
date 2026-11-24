#include "PluginProcessor.h"
#include "PluginEditor.h"

ChicagoBeatsBloodlineAudioProcessor::ChicagoBeatsBloodlineAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}

void ChicagoBeatsBloodlineAudioProcessor::prepareToPlay(double sampleRate, int) {
    currentSampleRate = sampleRate;
    fifo.fill(0.0f); fftData.fill(0.0f); fifoIndex = 0;
    low.store(0.0f); mid.store(0.0f); high.store(0.0f); peak.store(0.0f); kick.store(0.0f); midiPulse.store(0.0f);
}
void ChicagoBeatsBloodlineAudioProcessor::releaseResources() {}
bool ChicagoBeatsBloodlineAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void ChicagoBeatsBloodlineAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    const int channels = getTotalNumInputChannels();
    const int samples = buffer.getNumSamples();
    float blockPeak = 0.0f;
    for (int i = 0; i < samples; ++i) {
        float mono = 0.0f;
        for (int ch = 0; ch < channels; ++ch) {
            const float s = buffer.getReadPointer(ch)[i];
            mono += s;
            blockPeak = juce::jmax(blockPeak, std::abs(s));
        }
        mono /= (float) juce::jmax(1, channels);
        pushNextSampleIntoFifo(mono);
    }
    for (int ch = channels; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, samples);

    peak.store(juce::jmax(blockPeak, peak.load() * 0.82f));
    low.store(low.load() * 0.94f);
    mid.store(mid.load() * 0.92f);
    high.store(high.load() * 0.90f);
    kick.store(kick.load() * 0.78f);
    midiPulse.store(midiPulse.load() * 0.80f);

    for (const auto metadata : midi) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            lastMidiNote.store(msg.getNoteNumber());
            midiPulse.store(1.0f);
        }
    }
}

void ChicagoBeatsBloodlineAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept {
    if (fifoIndex == fftSize) { performFFT(); fifoIndex = 0; }
    fifo[(size_t) fifoIndex++] = sample;
}

void ChicagoBeatsBloodlineAudioProcessor::performFFT() noexcept {
    for (int i = 0; i < fftSize; ++i) {
        const float window = 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * (float)i / (float)(fftSize - 1));
        fftData[(size_t)i] = fifo[(size_t)i] * window;
    }
    std::fill(fftData.begin() + fftSize, fftData.end(), 0.0f);
    fft.performFrequencyOnlyForwardTransform(fftData.data());
    const float binHz = (float) currentSampleRate / (float) fftSize;
    auto band = [&](float loHz, float hiHz) {
        const int first = juce::jlimit(1, fftSize / 2 - 1, (int) std::floor(loHz / binHz));
        const int last = juce::jlimit(first + 1, fftSize / 2, (int) std::ceil(hiHz / binHz));
        float sum = 0.0f; int count = 0;
        for (int i = first; i < last; ++i) { sum += fftData[(size_t)i]; ++count; }
        return count > 0 ? juce::jlimit(0.0f, 1.0f, (sum / (float)count) / 55.0f) : 0.0f;
    };
    const float l = band(25.0f, 120.0f);
    low.store(juce::jmax(l, low.load() * 0.78f));
    mid.store(juce::jmax(band(120.0f, 4000.0f), mid.load() * 0.72f));
    high.store(juce::jmax(band(4000.0f, 15000.0f), high.load() * 0.68f));
    const float transient = juce::jmax(0.0f, l - previousLow * 1.12f);
    kick.store(juce::jmax(juce::jlimit(0.0f, 1.0f, transient * 3.0f), kick.load() * 0.60f));
    previousLow = l;
}

juce::AudioProcessorEditor* ChicagoBeatsBloodlineAudioProcessor::createEditor() { return new ChicagoBeatsBloodlineAudioProcessorEditor(*this); }
void ChicagoBeatsBloodlineAudioProcessor::getStateInformation(juce::MemoryBlock& dest) { juce::MemoryOutputStream out(dest, true); out.writeInt(1); }
void ChicagoBeatsBloodlineAudioProcessor::setStateInformation(const void* data, int size) { juce::ignoreUnused(data, size); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ChicagoBeatsBloodlineAudioProcessor(); }
