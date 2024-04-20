/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginPresetManagerAudioProcessor::PluginPresetManagerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, "TREE", createParameterLayout())
#endif
{
    tree.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    tree.state.setProperty("version", ProjectInfo::versionString, nullptr);
    presetManager = std::make_unique<PresetManager>(tree);
}

PluginPresetManagerAudioProcessor::~PluginPresetManagerAudioProcessor()
{
}

//==============================================================================
const juce::String PluginPresetManagerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginPresetManagerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginPresetManagerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginPresetManagerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginPresetManagerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginPresetManagerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginPresetManagerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginPresetManagerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginPresetManagerAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginPresetManagerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginPresetManagerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PluginPresetManagerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginPresetManagerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginPresetManagerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool PluginPresetManagerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginPresetManagerAudioProcessor::createEditor()
{
    return new PluginPresetManagerAudioProcessorEditor (*this);
}

//==============================================================================
void PluginPresetManagerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = tree.copyState();
    const auto xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void PluginPresetManagerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr){
        return;
    }
    const auto newTree = ValueTree::fromXml(*xmlState);
    tree.replaceState(newTree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginPresetManagerAudioProcessor();
}


AudioProcessorValueTreeState::ParameterLayout PluginPresetManagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    auto gainParam = std::make_unique<juce::AudioParameterFloat>(ParameterID{"GAIN_ID", 1}, "GAIN_NAME", NormalisableRange<float>(0.f, 1.f, 0.f), 0.f);
    auto thresholdinDBParam = std::make_unique<juce::AudioParameterFloat>(ParameterID{"THRESHOLD_ID", 1}, "THRESHOLD_NAME", NormalisableRange<float>(0.f, 1.f, 0.f), 1.f);
    auto attackInMSParam = std::make_unique<juce::AudioParameterFloat>(ParameterID{"ATTACK_ID", 1}, "ATTACK_NAME", NormalisableRange<float>(0.f, 1.f, 0.f), 0.2f);
    auto releaseInMSParam = std::make_unique<juce::AudioParameterFloat>(ParameterID{"RELEASE_ID", 1}, "RELEASE_NAME", NormalisableRange<float>(0.f, 1.f, 0.f), 0.2f);
    
    params.push_back(std::move(gainParam));
    params.push_back(std::move(thresholdinDBParam));
    params.push_back(std::move(attackInMSParam));
    params.push_back(std::move(releaseInMSParam));

    return {params.begin(), params.end()};
}
