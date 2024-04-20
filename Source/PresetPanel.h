/*
  ==============================================================================

    PresetPanel.h
    Created: 9 Apr 2024 1:15:34pm
    Author:  Tom Carpenter

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PresetPanel : public Component, Button::Listener, ComboBox::Listener
{
public:
    PresetPanel(PresetManager& pm) : presetManager(pm)
    {
        saveButton.setButtonText("Save");
        addAndMakeVisible(saveButton);
        saveButton.addListener(this);
        
        deleteButton.setButtonText("Delete");
        addAndMakeVisible(deleteButton);
        deleteButton.addListener(this);
        
        nextButton.setButtonText(">>");
        addAndMakeVisible(nextButton);
        nextButton.addListener(this);
        
        previousButton.setButtonText("<<");
        addAndMakeVisible(previousButton);
        previousButton.addListener(this);
        
        presetList.setTextWhenNothingSelected("No Preset Selected");
        addAndMakeVisible(presetList);
        presetList.addListener(this);
        
        loadPresetList();
    }
    
    ~PresetPanel()
    {
        saveButton.removeListener(this);
        deleteButton.removeListener(this);
        previousButton.removeListener(this);
        nextButton.removeListener(this);
        presetList.removeListener(this);
    }
    
private:
    void buttonClicked(Button* button) override
    {
        if (button == &saveButton)
        {
            fileChooser = std::make_unique<FileChooser>(
                "Please enter the name of the preset to save",
                PresetManager::defaultDirectory,
                "*." + PresetManager::extension
            );
            fileChooser->launchAsync(FileBrowserComponent::saveMode, [&](const FileChooser& chooser){
                const auto resultFile = chooser.getResult();
                presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                loadPresetList();
            });
        }
        
        
        if (button == &previousButton)
        {
            const auto index = presetManager.previousPreset();
            presetList.setSelectedItemIndex(index, dontSendNotification);
        }
        
        if (button == &nextButton)
        {
            const auto index = presetManager.nextPreset();
            presetList.setSelectedItemIndex(index, dontSendNotification);
        }
        
        if (button == &deleteButton)
        {
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
        }
        
    }
    
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == &presetList)
        {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
        }
        
    }
    
    void resized() override
    {
        const auto container = getLocalBounds().reduced(4);
        auto bounds  = container;
        
        saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2)).reduced(4));
        nextButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(4));
        presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4)).reduced(4));
        previousButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1)).reduced(4));
        deleteButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2)).reduced(4));
    }
    
    void loadPresetList()
    {
        presetList.clear(dontSendNotification);
        const auto allPresets = presetManager.getAllPresets();
        const auto currentPreset = presetManager.getCurrentPreset();
        presetList.addItemList(allPresets, 1);
        presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
    }

    
    std::unique_ptr<FileChooser> fileChooser;
    
    PresetManager& presetManager;
    TextButton saveButton, deleteButton, nextButton, previousButton;
    ComboBox presetList;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel);
};
