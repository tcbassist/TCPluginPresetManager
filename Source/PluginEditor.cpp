/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginPresetManagerAudioProcessorEditor::PluginPresetManagerAudioProcessorEditor (PluginPresetManagerAudioProcessor& p)
: presetPanel(p.getPresetManager()), AudioProcessorEditor (&p), genericAudioProcessorEditor(p), audioProcessor (p)
{
    addAndMakeVisible(genericAudioProcessorEditor);
    
    addAndMakeVisible(presetPanel);
    
    setResizable(true, true);

    setSize (400, 300);
}

PluginPresetManagerAudioProcessorEditor::~PluginPresetManagerAudioProcessorEditor()
{
}

//==============================================================================
void PluginPresetManagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginPresetManagerAudioProcessorEditor::resized()
{
    
    genericAudioProcessorEditor.setBounds(getLocalBounds().withSizeKeepingCentre(getLocalBounds().proportionOfWidth(0.9), proportionOfHeight(0.5)));
    presetPanel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1)));
}
