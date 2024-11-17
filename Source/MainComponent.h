/*
  ==============================================================================
    MainComponent.h
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"



    //This component lives inside our window, and this is where you should put all
    //your controls and content.

class MainComponent   : public AudioAppComponent
{
public:

    MainComponent();
    ~MainComponent();


    //Pure virtual overrides AudioAppComponent. instructs the source to get ready to play
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    //Pure virtual overrides AudioAppComponent. Called again and again to retrieve the next audio data block
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    //Pure virtual overrides AudioAppComponent. enables the source, once playback has halted, to release everything it no longer requires
    void releaseResources() override;


    //Customize the input images
    void paint (Graphics& g) override;
    //Rescaling of program window components
    void resized() override;


private:

    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{ 100 }; // store 100 files in cache

    int channelL = 0;
    int channelR = 1;

    PlaylistComponent playlistComponent{formatManager};
    DJAudioPlayer playerLeft{formatManager};
    DeckGUI deckGUILeft{&playerLeft,&playlistComponent, formatManager, thumbCache, channelL};

    DJAudioPlayer playerRight{ formatManager };
    DeckGUI deckGUIRight{&playerRight, &playlistComponent, formatManager, thumbCache, channelR};

    Label waveformLabel;
    Label posLabel;
    Label widgetLabel;
    Label playlistLabel;

    MixerAudioSource mixerSource;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent); 
};
