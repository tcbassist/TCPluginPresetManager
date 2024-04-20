/*
  ==============================================================================

    PresetManager.h
    Created: 9 Apr 2024 1:33:43pm
    Author:  Tom Carpenter

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PresetManager : ValueTree::Listener
{
public:
    PresetManager(AudioProcessorValueTreeState&);
    
    ~PresetManager();
    
    void savePreset(const String& presetName);
    
    void deletePreset(const String& presetName);
    
    void loadPreset(const String& presetName);
    
    int nextPreset();
    
    int previousPreset();
    
    StringArray getAllPresets();

    String getCurrentPreset();
    
    static const File defaultDirectory;
    static const String extension;
    static const String presetNameProperty;

    String currentPreset;
    
private:
    void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override; 
    
    AudioProcessorValueTreeState& treeRef;
};
