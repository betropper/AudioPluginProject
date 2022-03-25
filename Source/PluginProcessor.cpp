/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PitchDetectorPluginAudioProcessor::PitchDetectorPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

PitchDetectorPluginAudioProcessor::~PitchDetectorPluginAudioProcessor()
{
}

//==============================================================================
const juce::String PitchDetectorPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PitchDetectorPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PitchDetectorPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PitchDetectorPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PitchDetectorPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PitchDetectorPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PitchDetectorPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PitchDetectorPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PitchDetectorPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void PitchDetectorPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PitchDetectorPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PitchDetectorPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PitchDetectorPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PitchDetectorPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* channelData = buffer.getReadPointer(0);

    updatePeriodBounds();
    int runningSum = 0;
    timeCount++;
    for (auto tau = minPeriod; tau <= maxPeriod; ++tau) {
        const auto yinIndex = tau - minPeriod;
        for (auto i = 0; i + tau < buffer.getNumSamples(); ++i) {
            //in juce, inputChannelData stores the samples for both the left and right channels. I'm just using 0, the left channel, in this formula.
            const auto delta = channelData[i] - channelData[i + 1];
            yinStorageData[yinIndex] += (delta * delta);
        }
        runningSum += yinStorageData[yinIndex];
        yinStorageData[yinIndex] *= (static_cast<float>(tau) / runningSum);
    }
    periodEstimate = absoluteThreshold();
}

//==============================================================================
bool PitchDetectorPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PitchDetectorPluginAudioProcessor::createEditor()
{
    return new PitchDetectorPluginAudioProcessorEditor (*this);
}

//==============================================================================
void PitchDetectorPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PitchDetectorPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void PitchDetectorPluginAudioProcessor::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples)
{
    /*
    * 
    * NOTE FOR MATT: Everything here involving audioDeviceIOCallback, audioDeviceAboutToStart, and audioDevice stopped did not appear to work within the plugin format.
    * However, I was able to put all this code into the above processblock instead and convert it nice. However, I'm keeping these functions here for now in case we want to tackle other kinds of input.
    updatePeriodBounds();
    int runningSum = 0;
    timeCount++;
    for (auto tau = minPeriod; tau <= maxPeriod; ++tau) {
        const auto yinIndex = tau - minPeriod;
        for (auto i = 0; i + tau < numSamples; ++i) {
            //in juce, inputChannelData stores the samples for both the left and right channels. I'm just using 0, the left channel, in this formula.
            const auto delta = inputChannelData[0][i] - inputChannelData[0][i + 1];
                yinStorageData[yinIndex] += (delta * delta);
        }
        runningSum += yinStorageData[yinIndex];
        yinStorageData[yinIndex] *= (static_cast<float>(tau) / runningSum);
    }
    periodEstimate = absoluteThreshold();
    */
}

//These two have to be included in order to inherit the AudioIODeviceCallback method in PluginProcessor.h.
void PitchDetectorPluginAudioProcessor::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
}

void PitchDetectorPluginAudioProcessor::audioDeviceStopped()
{

}

void PitchDetectorPluginAudioProcessor::updatePeriodBounds() {
    //The minHZ we should check must never be a negative number - there's no such thign as a negative HZ value.
    jassert(minHz > 0);
    //NOTE FOR MATT: I couldn't figure out how to mimic the 'math::periodInSamples' function and the 'math::freqFromPeriod' that seems to be part of something the Lime framework has that Juce does not.
    //If you can figure out which equations those are, right now I'm just testing a random max and min.
    maxPeriod = 100;
    minPeriod = 0;
}
int PitchDetectorPluginAudioProcessor::absoluteThreshold() const {
    const auto maxYinIdx = maxPeriod - minPeriod;

    const auto tau = [this, maxYinIdx, thresh = confidenceThresh]
    {
        for (auto t = 0; t <= maxYinIdx; ++t)
        {
            if (yinStorageData[t] < thresh)
            {
                while (t + 1 <= maxYinIdx && yinStorageData[t + 1] < yinStorageData[t])
                {
                    ++t;
                }

                return t;
            }
        }

        return maxYinIdx + 1;
    }();

    if (tau > maxYinIdx || yinStorageData[tau] >= confidenceThresh)
        return 0;

    return tau;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchDetectorPluginAudioProcessor();
}
