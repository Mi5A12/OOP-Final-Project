/*
  ==============================================================================
    DeckGUI.h
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "PlaylistComponent.h"

//This part manages the application's graphical user interface for every channel.

class DeckGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public TableListBoxModel,
    public Timer
{
public:

    DeckGUI(DJAudioPlayer* player,
        PlaylistComponent* playlistComponent,
        AudioFormatManager& formatManagerToUse,
        AudioThumbnailCache& cacheToUse, 
        int channelToUse);
    ~DeckGUI();


    //Customize the input images
    void paint(Graphics&) override;
    //Components on the application window are resized
    void resized() override;


    //Override of pure virtual listener button.
    //Called when the button is clicked, allowing interacion of play,pause,next buttons with the player
    void buttonClicked(Button*) override;

    //Pure virtual overrides for Slider::Listener.
    //Called when the slider's value is changed, allowing interacion of vol,speed,playback sliders with the player
    void sliderValueChanged(Slider* slider) override;


    //Pure virtual overrides TableListBoxModel.To open the following table in the GUI
    //Returns the number of rows currently in the table
    int getNumRows() override;
    //TableListBoxModel pure virtual override.
    //Draws the background behind one of the table's rows in the GUI's for up next table
    void paintRowBackground(Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override;
    //TableListBoxModel pure virtual override.
     //Draws a cell for the next table in the GUI, which is used to enter details in cells.
    void paintCell(Graphics&,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;


    //Completely virtual timer override.To enable a callback for a waveform visual update
    void timerCallback() override;

    
private: 

    //Create Buttons
    TextButton playButton{ "PLAY" };
    TextButton stopButton{ "PAUSE" };
    TextButton nextButton{ "LOAD" };

    //Create Sliders 
    Slider volSlider{"volume"};
    Slider speedSlider{"speed"};
    Slider posSlider{"position"};

    //Add labels to sliders 
    Label volLabel; 
    Label speedLabel;

    //Control visual theme
    LookAndFeel_V4 lookandfeel;

    //Construct a player connected to the GUI
    DJAudioPlayer* player;
    //Create a GUI-connected playlist component.
    PlaylistComponent* playlistComponent;

    // Produce a graphic waveform
    WaveformDisplay waveformDisplay;

    //Make a table with the playlist's upcoming song list in it.
    TableListBox upNext;

    //variable representing the GUI's channel (0=Left, 1=Right)
    int channel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI);
};
