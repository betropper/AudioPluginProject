/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//Quick way to do a string conversion.
#include <string>

//==============================================================================
PitchDetectorPluginAudioProcessorEditor::PitchDetectorPluginAudioProcessorEditor (PitchDetectorPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize(400, 300);
    //Start the timer.
    startTimerHz(24);

}

PitchDetectorPluginAudioProcessorEditor::~PitchDetectorPluginAudioProcessorEditor()
{
}
//This is the timer. Every time it's called, we'll change the text to the value of our periodEstimate.
void PitchDetectorPluginAudioProcessorEditor::timerCallback() {
    //Repaint every time the timer is called.
    repaint();
};

//==============================================================================
void PitchDetectorPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //Adding in the label for period here. Not needed yet, maybe later.
    //addAndMakeVisible(periodLabel);
    juce::String periodString = "Period: " + juce::String(audioProcessor.periodEstimate);
    g.drawText(periodString, getLocalBounds(), juce::Justification::centred, true);
    //g.drawText("The count is: " + juce::String(audioProcessor.timeCount), getLocalBounds(), juce::Justification::centred, true);
    //periodLabel.setText(periodString, juce::dontSendNotification);
    //periodLabel.setJustificationType(juce::Justification::horizontallyCentred);
}

void PitchDetectorPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
