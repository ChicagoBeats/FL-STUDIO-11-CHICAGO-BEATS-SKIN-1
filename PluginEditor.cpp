#include "PluginEditor.h"
#include <BinaryData.h>

namespace {
const juce::Colour black { 0xff080609 }, red { 0xffed1837 }, crimson { 0xff75101f }, gold { 0xffe6b75e };
void panel(juce::Graphics& g, juce::Rectangle<float> r) {
    g.setColour(juce::Colour(0xff13080d)); g.fillRoundedRectangle(r, 8.0f);
    g.setColour(juce::Colour(0xff76202d)); g.drawRoundedRectangle(r, 8.0f, 1.2f);
}
}

ChicagoBeatsBloodlineAudioProcessorEditor::ChicagoBeatsBloodlineAudioProcessorEditor(ChicagoBeatsBloodlineAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p) {
    carImage = juce::ImageFileFormat::loadFrom(BinaryData::batmobile_car_reference_png, (size_t)BinaryData::batmobile_car_reference_pngSize);
    speakerImage = juce::ImageFileFormat::loadFrom(BinaryData::speaker_room_reference_jpg, (size_t)BinaryData::speaker_room_reference_jpgSize);
    auto setup = [this](juce::TextButton& b) { addAndMakeVisible(b); b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff3b0b14)); b.setColour(juce::TextButton::textColourOffId, gold); };
    for (auto* b : { &cruiseButton, &burnoutButton, &donutButton, &holdButton }) setup(*b);
    cruiseButton.onClick = [this] { carMode = CarMode::cruise; repaint(); };
    burnoutButton.onClick = [this] { carMode = CarMode::burnout; repaint(); };
    donutButton.onClick = [this] { carMode = CarMode::donut; repaint(); };
    holdButton.onClick = [this] { carMode = CarMode::hold; repaint(); };
    setSize(1120, 760);
    startTimerHz(30);
}

void ChicagoBeatsBloodlineAudioProcessorEditor::timerCallback() {
    visualTime += 1.0f / 30.0f;
    const float bass = processor.getLow();
    if (carMode == CarMode::cruise) {
        carPosition += carDirection * (0.003f + bass * 0.002f);
        if (carPosition > 0.78f) carDirection = -1.0f;
        if (carPosition < 0.08f) carDirection = 1.0f;
        carRotation = 0.0f;
    } else if (carMode == CarMode::burnout) {
        carRotation += 0.018f + bass * 0.06f;
    } else if (carMode == CarMode::donut) {
        carRotation += 0.075f + bass * 0.11f;
    }
    repaint();
}

void ChicagoBeatsBloodlineAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(black);
    auto area = getLocalBounds().toFloat().reduced(14.0f);
    auto header = area.removeFromTop(62.0f); drawHeader(g, header);
    area.removeFromTop(10.0f);
    auto runway = area.removeFromTop(206.0f); drawCarRunway(g, runway);
    area.removeFromTop(10.0f);
    auto lower = area;
    auto left = lower.removeFromLeft(lower.getWidth() * 0.48f);
    lower.removeFromLeft(10.0f);
    drawPiano(g, left);
    drawSpeakers(g, lower);
}

void ChicagoBeatsBloodlineAudioProcessorEditor::drawHeader(juce::Graphics& g, juce::Rectangle<float> r) {
    panel(g, r); g.setColour(gold); g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("♛  CHICAGO BEATS BLOODLINE", r.withTrimmedLeft(18).withTrimmedTop(4), juce::Justification::centredLeft);
    g.setColour(red); g.setFont(11.0f); g.drawText("REAL-TIME AUDIO • MIDI • MATRIX VISUAL SYSTEM", r.withTrimmedLeft(20).withTrimmedTop(34), juce::Justification::centredLeft);
    g.setColour(processor.getPeak() > 0.02f ? juce::Colour(0xff47e28a) : gold);
    g.drawText(processor.getPeak() > 0.02f ? "SIGNAL ACTIVE" : "WAITING FOR SIGNAL", r.withTrimmedRight(18), juce::Justification::centredRight);
}

void ChicagoBeatsBloodlineAudioProcessorEditor::drawCarRunway(juce::Graphics& g, juce::Rectangle<float> r) {
    panel(g, r); auto inner = r.reduced(10.0f); g.saveState(); g.reduceClipRegion(inner.toNearestInt());
    g.setColour(juce::Colour(0xff09070a)); g.fillRect(inner);
    for (int i=0;i<60;++i) { const float x = std::fmod(i*83.0f + visualTime*42.0f, inner.getWidth()); const float y = std::fmod(i*37.0f + visualTime*76.0f, inner.getHeight()); g.setColour(juce::Colour(0x36ff1736)); g.setFont(11.0f); g.drawText((i%2)==0?"1":"0", (int)(inner.getX()+x), (int)(inner.getY()+y), 12, 12, juce::Justification::centred); }
    g.setColour(crimson); g.drawLine(inner.getX(), inner.getBottom()-18, inner.getRight(), inner.getBottom()-18, 2.0f);
    if (carImage.isValid()) {
        auto car = juce::Rectangle<float>(inner.getX()+carPosition*inner.getWidth(), inner.getY()+14, inner.getWidth()*0.24f, inner.getHeight()-34);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        if (carMode == CarMode::donut || carMode == CarMode::burnout) {
            g.saveState(); g.addTransform(juce::AffineTransform::rotation(carRotation, car.getCentreX(), car.getCentreY()));
            g.drawImageWithin(carImage, (int)car.getX(), (int)car.getY(), (int)car.getWidth(), (int)car.getHeight(), juce::RectanglePlacement::centred);
            g.restoreState();
        } else g.drawImageWithin(carImage, (int)car.getX(), (int)car.getY(), (int)car.getWidth(), (int)car.getHeight(), juce::RectanglePlacement::centred);
    }
    if (carMode == CarMode::burnout || carMode == CarMode::donut) {
        for (int i=0;i<18;++i) { float x=inner.getX()+std::fmod(i*53.0f+visualTime*28.0f,inner.getWidth()); float y=inner.getBottom()-25.0f-(float)((i*19)%70); g.setColour(juce::Colour(0x22d9d9d9)); g.fillEllipse(x,y,18.0f+(i%4)*8.0f,10.0f+(i%3)*7.0f); }
    }
    g.restoreState();
    g.setColour(gold); g.setFont(11.0f); g.drawText("01 / BATMOBILE RUNWAY  •  28-INCH RIM ART DIRECTION", r.toNearestInt().reduced(12,5), juce::Justification::topLeft);
}

void ChicagoBeatsBloodlineAudioProcessorEditor::drawPiano(juce::Graphics& g, juce::Rectangle<float> r) {
    panel(g,r); auto a=r.reduced(12); g.setColour(gold); g.setFont(12.0f); g.drawText("02 / PLAYLIST PIANO • MIDI FALL", a.removeFromTop(22), juce::Justification::centredLeft);
    auto keyboard=a.removeFromBottom(48); const int keys=36; const float kw=keyboard.getWidth()/keys;
    for(int i=0;i<keys;++i){ bool blackKey=(i%12==1||i%12==3||i%12==6||i%12==8||i%12==10); g.setColour(blackKey?juce::Colour(0xff170b10):juce::Colour(0xffd8c9c6)); g.fillRect(keyboard.getX()+i*kw,keyboard.getY(),kw-1,keyboard.getHeight()); }
    g.setColour(juce::Colour(0x44ed1837)); for(int i=0;i<12;++i){float x=a.getX()+std::fmod(i*49.0f+visualTime*35.0f,a.getWidth());float y=a.getY()+std::fmod(i*31.0f+visualTime*(45.0f+processor.getLow()*100.0f),a.getHeight());g.fillRoundedRectangle(x,y,9.0f,18.0f+(i%3)*8.0f,3.0f);}
    g.setColour(red); g.drawText("NOTE INPUT: " + juce::String(juce::MidiMessage::getMidiNoteName(processor.getLastMidiNote(), true, true, 4)), a.toNearestInt().removeFromBottom(18), juce::Justification::centredLeft);
}

void ChicagoBeatsBloodlineAudioProcessorEditor::drawSpeakers(juce::Graphics& g, juce::Rectangle<float> r) {
    panel(g,r); auto a=r.reduced(10); g.setColour(gold); g.setFont(12.0f); g.drawText("03 / BLOODLINE SPEAKER ARRAY + SPECTRUM", a.removeFromTop(22), juce::Justification::centredLeft);
    auto imageArea=a;
    if(speakerImage.isValid()) g.drawImageWithin(speakerImage,(int)imageArea.getX(),(int)imageArea.getY(),(int)imageArea.getWidth(),(int)imageArea.getHeight(),juce::RectanglePlacement::stretchToFit);
    g.setColour(juce::Colour(0x99050008)); g.fillRect(imageArea);
    const float bands[3]={processor.getLow(),processor.getMid(),processor.getHigh()};
    const juce::String labels[3]={"LOW / SUB","MID","HIGH"};
    for(int b=0;b<3;++b){auto bar=juce::Rectangle<float>(imageArea.getX()+12+b*imageArea.getWidth()/3,imageArea.getBottom()-22,imageArea.getWidth()/3-20,12);g.setColour(juce::Colour(0xff250b12));g.fillRoundedRectangle(bar,4);bar.setWidth(bar.getWidth()*juce::jlimit(0.0f,1.0f,bands[b]));g.setColour(b==0?red:gold);g.fillRoundedRectangle(bar,4);g.setColour(juce::Colours::white);g.setFont(9.0f);g.drawText(labels[b],(int)(imageArea.getX()+12+b*imageArea.getWidth()/3),(int)imageArea.getY()+4,(int)(imageArea.getWidth()/3-20),12,juce::Justification::centredLeft);}
}
void ChicagoBeatsBloodlineAudioProcessorEditor::resized() {
    auto r=getLocalBounds().reduced(14); r.removeFromTop(62+10+206+10); auto left=r.removeFromLeft((int)(r.getWidth()*0.48f)); r.removeFromLeft(10);
    auto controls=left.removeFromBottom(1); juce::ignoreUnused(controls);
    const int buttonY=14+62+10+206-29; cruiseButton.setBounds(18,buttonY,112,22); burnoutButton.setBounds(136,buttonY,86,22); donutButton.setBounds(228,buttonY,76,22); holdButton.setBounds(310,buttonY,58,22);
}
