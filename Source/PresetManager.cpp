/*
  ==============================================================================

    PresetManager.cpp
    Created: 9 Apr 2024 1:46:39pm
    Author:  Tom Carpenter

  ==============================================================================
*/

#include "PresetManager.h"

const File PresetManager::defaultDirectory
{ File::getSpecialLocation(File::SpecialLocationType::commonDocumentsDirectory)
    .getChildFile(ProjectInfo::companyName)
    .getChildFile(ProjectInfo::projectName)
};
const String PresetManager::extension{ "preset" };
const String PresetManager::presetNameProperty{ "presetName" };

PresetManager::PresetManager(AudioProcessorValueTreeState& tree) : treeRef(tree)
{
    if (!defaultDirectory.exists()){
        {
            const auto result = defaultDirectory.createDirectory();
            if (result.failed())
            {
                DBG("Could not create Preset Directory");
                jassertfalse;
            }
        }
    }
    currentPreset = treeRef.state.getPropertyAsValue(presetNameProperty, nullptr).toString();
    treeRef.state.addListener(this);
}

PresetManager::~PresetManager()
{
    treeRef.state.removeListener(this);
}

void PresetManager::savePreset(const String& presetName)
{
    if (presetName.isEmpty()){
        return;
    }
    
    const auto xmlCurrentState = treeRef.copyState().createXml();
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (!xmlCurrentState->writeTo(presetFile))
    {
        jassertfalse;
    }

    currentPreset = presetName;
}

void PresetManager::deletePreset(const String& presetName)
{
    if (presetName.isEmpty()){
        return;
    }
    
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    
    if (!presetFile.deleteFile())
    {
        DBG("Preset File does not exist");
        jassertfalse;
        return;
    }
    
    currentPreset = "";
}

void PresetManager::loadPreset(const String& presetName)
{
    if (presetName.isEmpty())
        return;
    
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    
    if (!presetFile.existsAsFile())
    {
        jassertfalse;
        return;
    }
    
    XmlDocument xmlDocument{ presetFile };
    const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());
    treeRef.replaceState(valueTreeToLoad);
    currentPreset = presetName;
}

int PresetManager::nextPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
    {
        return -1;
    }
    
    const auto currentIndex = allPresets.indexOf(currentPreset);
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
    const auto nameOfNextPreset = allPresets.getReference(nextIndex);
    loadPreset(nameOfNextPreset);
    return nextIndex;
}

int PresetManager::previousPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
    {
        return -1;
    }
    
    const auto currentIndex = allPresets.indexOf(currentPreset);
    const auto nextIndex = currentIndex - 1 < 0 ? (allPresets.size() - 1) : 0;
    const auto nameOfNextPreset = allPresets.getReference(nextIndex);
    loadPreset(nameOfNextPreset);
    return nextIndex;
}

StringArray PresetManager::getAllPresets()
{
    StringArray presets;
    const auto fileArray = defaultDirectory.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*" + extension);
    for (const auto& file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }
    return presets;
}

String PresetManager::getCurrentPreset()
{
    return currentPreset;
}

void PresetManager::valueTreeRedirected(ValueTree& treeWhichHasBeenChanged)
{
    currentPreset = treeRef.state.getPropertyAsValue(presetNameProperty, nullptr).toString();
}
