    /*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class PitchDetectorPluginAudioProcessor  : public juce::AudioProcessor, public juce::AudioIODeviceCallback
{
public:
    //==============================================================================
    PitchDetectorPluginAudioProcessor();
    ~PitchDetectorPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //Declare a function that overides juce::AudioIODeviceCallback's default function to do our work in.
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    //Defining the period estimate here so that plugineditor.cpp can see it.
    float periodEstimate;
    //A test for me - just how many times the timer has counted.
    int timeCount = 0;
private:
    //Declare the function to update the bounds of the period we're checking in our algorithm.
    inline void updatePeriodBounds();
    inline int absoluteThreshold() const;
    //Declare a value to hold the samplerate that we can set at will.
    double sampleRate{ 0.0 };
    //Define the minium and maximum period that the value could be, to check between all periods.
    int minPeriod{ 0 }, maxPeriod{ 0 };
    //Setting the minHz. This value depend on what device we're using.
    int minHz{ 60 };
    //As a default we will take a max of 2000 samples.
    int yinLength = 2000;
    float yinStorageData[2000] = { };
    //Value should be between 0 and 1. The lower this is, the stricter the algorithm is for termining pitch values. Code suggested between .1 to .2.
    float confidenceThresh{ 0.15 };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchDetectorPluginAudioProcessor)
};
