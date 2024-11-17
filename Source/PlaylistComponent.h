/*
  ==============================================================================
    PlaylistComponent.h
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>


//This part manages the playlist, which is where OS to app files are uploaded.


class PlaylistComponent : public juce::Component,
    public TableListBoxModel,
    public AudioSource,
    public Button::Listener,
    public FileDragAndDropTarget,
    public TextEditor::Listener
{
public:

    PlaylistComponent(AudioFormatManager& formatManager);
    ~PlaylistComponent() override;


    // Modify the input images
    void paint (juce::Graphics&) override;
    //Rescaling of program window components
    void resized() override;


    //TableListBoxModel pure virtual override.
    //gives back the number of rows that are present in the table.
    int getNumRows() override;
    //TableListBoxModel pure virtual override.
    //Creates the backdrop behind one of the table's rows.
    void paintRowBackground(Graphics&,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override; 
    //TableListBoxModel pure virtual override.
    //Draws a cell, which is then used to enter information in cells.
    void paintCell(Graphics &,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;
    //The TableListBoxModel method has been overridden.
    //To create & update custom components such as buttons to go in a cell
    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool isRowSelected,
        Component* existingComponentToUpdate) override;


    //Override placeholder function for Audio Source component to instruct source to get ready to play
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    //Override placeholder function for Audio Source component to retrieve next audio data blocks
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    //Override placeholder function to release everything Audio Source component no longer needs
    void releaseResources();


    //Override of pure virtual listener button.
    //triggered upon button click, enabling users to add files to deck
    void buttonClicked(Button* button) override;


    //FileDragAndDropTarget pure virtual override.
    //To find out if this target is interested in the files being supplied, please call back.
    //Allows files to be dropped and dragged into the area if set to true.
    bool isInterestedInFileDrag(const StringArray& files) override;
    //FileDragAndDropTarget pure virtual override.
    //Callback to show that the files were dumped onto this component by the user.
    //files after they have been dropped (mouse released).
    void filesDropped(const StringArray& files, int x, int y) override;


    //Override of TextEditor::Listener function to be invoked each time a user modifies the text within the object
    void textEditorTextChanged(TextEditor&) override;


    //Songs to add to the Left Channel Player, DeckGUI-utilized vector
    std::vector<std::string> playListL;
    //Song vector that DeckGUI will use to add tracks to the Right Channel Player
    std::vector<std::string> playListR;


private:

    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;

    //Playlist shown as a list in a table
    TableListBox tableComponent;

    //vectors for storing metadata about music files
    std::vector<std::string> inputFiles;
    std::vector<std::string> interestedFiles;
    std::vector<std::string> trackTitles; 
    std::vector<std::string> interestedTitle;
    std::vector<int> trackDurations;
    std::vector<int> interestedDuration;

    // To enable searching, include a search bar and label.
    TextEditor searchBar;
    Label searchLabel;

    //using user-defined variables to handle data
    void addToChannelList(std::string filepath, int channel);
    void getAudioLength(URL audioURL);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};

