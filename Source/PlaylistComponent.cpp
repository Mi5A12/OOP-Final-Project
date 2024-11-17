/*
  ==============================================================================
    PlaylistComponent.cpp
  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(AudioFormatManager& _formatManager)
    : formatManager(_formatManager)
{
    //assemble the playlist library table
    tableComponent.getHeader().addColumn("Track Title:",1, 250);
    tableComponent.getHeader().addColumn("Duration:", 2, 100);
    tableComponent.getHeader().addColumn("Add to L:", 3, 100);
    tableComponent.getHeader().addColumn("Add to R:", 4, 100);
    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);

    //Include a listener and a search bar.
    addAndMakeVisible(searchBar);
    searchBar.addListener(this);
    //give the search bar a label
    addAndMakeVisible(searchLabel);
    searchLabel.setText("Search: ", juce::dontSendNotification);

}

PlaylistComponent::~PlaylistComponent()
{
}


void PlaylistComponent::paint (juce::Graphics& g)
{
}

void PlaylistComponent::resized()
{
    double rowH = getHeight() / 8;
    double colW = getWidth() / 6;

    //locate the search functionality
    searchLabel.setBounds(0, 0, colW, rowH);
    searchBar.setBounds(60, 0, colW * 5.5, rowH);
    // adjust the table's location
    tableComponent.setBounds(0, rowH, getWidth(), rowH * 7);

}


int PlaylistComponent::getNumRows()
{
    return interestedTitle.size(); // the filtered vector's length
}

void PlaylistComponent::paintRowBackground(Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange); // If chosen, the row's background color will be orange.
    }
    else {
        g.fillAll(juce::Colours::grey); //Rows that are not selected will be gray in color.
    }
}

void PlaylistComponent::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    // Add Track Title Name to the top row.
    if (columnId == 1)
    {
        g.drawText(interestedTitle[rowNumber],
            1, rowNumber,
            width - 4, height,
            Justification::centredLeft,
            true);
    }
    // Draw the second column's duration in seconds.
    if (columnId == 2)
    {
        g.drawText(std::to_string(interestedDuration[rowNumber]) + "s",
            1, rowNumber,
            width - 4, height,
            Justification::centredLeft,
            true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{
    // To add buttons to the Left channel deck, make buttons for every line in the Third Column.
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Add to L" };
            String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->setColour(TextButton::buttonColourId, juce::Colours::darkslategrey);
        }
    }
    // To add buttons to the right channel deck, make buttons for every line in the third column.
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "Add to R" };
            // increase id by 1000 to enable alternative channel processing for buttons
            String id{ std::to_string(rowNumber + 1000) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
            btn->setColour(TextButton::buttonColourId, juce::Colours::darkslategrey);
        }
    }
    return existingComponentToUpdate;
}


// Pure virtual functions for AudioSource
void PlaylistComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate){}
void PlaylistComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill){}
void PlaylistComponent::releaseResources(){}


void PlaylistComponent::buttonClicked(Button* button)
{
    //get the button's id
    int id = std::stoi(button->getComponentID().toStdString());
    //id should be assigned to the left channel GUI player if it is less than 1000.
    if (id < 1000)
    {
        addToChannelList(interestedFiles[id], 0);
    }
    //it should be assigned to the appropriate channel GUI player if id is 1000 or greater.
    else
    {
        addToChannelList(interestedFiles[id - 1000], 1);
    }
}


bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true; //enables the dropping and dragging of files
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    //act in the event that files are dropped (mouse released with files)
    for (String filename : files)
    {
        //get the filepath and file name for every file URL.
        std::string filepath = String(filename).toStdString();
        std::size_t startFilePos = filepath.find_last_of("\\");
        std::size_t startExtPos = filepath.find_last_of(".");
        std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
        std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);

        // update vectors with file information
        inputFiles.push_back(filepath);
        trackTitles.push_back(file);

        //calculate the file's audio length and update the vectors for the file's information.
        getAudioLength(URL{ File{filepath} });

    }
    //Start the interested titles as a complete list.
    //When text is input in the search bar, this will be updated.
    interestedTitle = trackTitles;
    interestedFiles = inputFiles;

    //add additional files to the music library database.
    tableComponent.updateContent();
}


void PlaylistComponent::textEditorTextChanged(TextEditor& textEditor)
{
    //Clear the vectors that will be used for the table anytime the search box is altered.
    interestedTitle.clear();
    interestedDuration.clear();
    interestedFiles.clear();

    // begin from the initial library list's position 0 and increase until the list's final element
    int pos = 0;
    for (std::string track : trackTitles)
    {   
        //verify whether the text entered in the search field is a substring of the track name that we are presently analyzing.
        if (track.find(searchBar.getText().toStdString()) != std::string::npos)
        {
            //Push the item to the vector that is used to display values in the table if there is a match.
            interestedTitle.push_back(trackTitles[pos]);
            interestedDuration.push_back(trackDurations[pos]);
            interestedFiles.push_back(inputFiles[pos]);
        }
        ++pos; 
    }
    //after looping, update the table's contents.
    tableComponent.updateContent();
}


// Add a music file to the playlist for the corresponding Left/Right channel.
void PlaylistComponent::addToChannelList(std::string filepath, int channel)
{
    if (channel == 0) //left
    {
        playListL.push_back(filepath);
    }
    if (channel == 1) //right
    {
        playListR.push_back(filepath);
    }
}

// get details about audio length
void PlaylistComponent::getAudioLength(URL audioURL)
{
    double trackLen = 0.0;

    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr)
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
            true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        double trackLen = transportSource.getLengthInSeconds(); // determine the audio's duration
        trackDurations.push_back(trackLen); // extend the vector's audio duration
    }

    //Initialize the entire list of interested durations.
    //When text is input in the search bar, this will be updated.
    interestedDuration = trackDurations;
}
