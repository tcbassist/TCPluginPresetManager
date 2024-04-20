/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PresetPanel.h"

//==============================================================================
/**
*/
class PluginPresetManagerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PluginPresetManagerAudioProcessorEditor (PluginPresetManagerAudioProcessor&);
    ~PluginPresetManagerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GenericAudioProcessorEditor genericAudioProcessorEditor; 
    
    PresetPanel presetPanel;
        
    PluginPresetManagerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginPresetManagerAudioProcessorEditor)
};
