/*
  ==============================================================================
    This file was auto-generated!
  ==============================================================================
*/

#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (800, 600);

    // In order to open input channels on certain systems, you must request permissions.
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        // Indicate how many input and output channels we wish to open.
        setAudioChannels (0, 2);
    }

    // Register file types that JUCE has enabled
    formatManager.registerBasicFormats();

    // Include and display application components
    addAndMakeVisible(deckGUILeft);
    addAndMakeVisible(deckGUIRight);
    addAndMakeVisible(playlistComponent);

    // Include labels and alter their visual representations
    addAndMakeVisible(waveformLabel);
    waveformLabel.setText("Waveforms", juce::dontSendNotification);
    waveformLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    waveformLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(posLabel);
    posLabel.setText("Playback", juce::dontSendNotification);
    posLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    posLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widgetLabel);
    widgetLabel.setText("Controls", juce::dontSendNotification);
    widgetLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    widgetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(playlistLabel);
    playlistLabel.setText("Drag Files here...", juce::dontSendNotification);
    playlistLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    playlistLabel.setJustificationType(juce::Justification::centred);

}

MainComponent::~MainComponent()
{
    // Doing so clears the audio source and turns off the audio device.
    shutdownAudio();
}


void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    playlistComponent.prepareToPlay(samplesPerBlockExpected, sampleRate);

    playerLeft.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerRight.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&playerLeft, false); 
    mixerSource.addInputSource(&playerRight, false);

 }

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // When the audio device stops or is restarted as a result of a setting change, this will be called.

    playlistComponent.releaseResources();

    playerLeft.releaseResources();
    playerRight.releaseResources();
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    double rowH = getHeight() / 10;
    double colW = getWidth() / 7;

    //place labels on the screen's left side.
    waveformLabel.setBounds(0, 0, colW, rowH*2);
    posLabel.setBounds(0, rowH*2, colW, rowH);
    widgetLabel.setBounds(0, rowH*3, colW, rowH*3);
    playlistLabel.setBounds(0, rowH*6, colW, rowH*3);

    //add GUIs
    deckGUILeft.setBounds(colW, 0, colW * 3, rowH*6);
    deckGUIRight.setBounds(colW * 4, 0, colW * 3, rowH * 6);

    //add playlist
    playlistComponent.setBounds(colW, rowH *6, colW * 6, rowH * 4);

}

