/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/

//I extended the juce Timer class to allow the functions to be continuously run over time.
class PitchDetectorPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    PitchDetectorPluginAudioProcessorEditor (PitchDetectorPluginAudioProcessor&);
    ~PitchDetectorPluginAudioProcessorEditor() override;

    //==============================================================================
    //We inherited the timer class. This method will help us count the time. Overriden in the PluginEditor file.
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PitchDetectorPluginAudioProcessor& audioProcessor;
    //defining my label to display the period here.
    juce::Label periodLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchDetectorPluginAudioProcessorEditor)
};
