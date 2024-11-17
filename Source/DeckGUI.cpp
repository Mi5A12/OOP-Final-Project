/*
  ==============================================================================
    DeckGUI.cpp
  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"    
#include <cmath>
#include "PlaylistComponent.h"


DeckGUI::DeckGUI(DJAudioPlayer* _player,
                PlaylistComponent* _playlistComponent,
                AudioFormatManager& formatManagerToUse,
                AudioThumbnailCache& cacheToUse, 
                int channelToUse
                ) : player(_player), 
                    playlistComponent(_playlistComponent),
                    waveformDisplay(formatManagerToUse,cacheToUse), 
                    channel(channelToUse)
{

    //create buttons and listeners for each graphical user interface
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(nextButton);
    playButton.addListener(this);
    stopButton.addListener(this);
    nextButton.addListener(this);

    //create, format, label, and add listeners to each GUI's sliders.
    addAndMakeVisible(posSlider);
    posSlider.addListener(this);
    posSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    posSlider.setRange(0.0, 1.0);
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(volSlider);
    volSlider.addListener(this);
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(0.5); //half of the maximum loudness at default
    volSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(volLabel);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(speedSlider);
    speedSlider.addListener(this);
    speedSlider.setRange(0.5, 2, 0); //max speed 2x, min half speed
    speedSlider.setValue(1); //use 1x as the default speed
    speedSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalDrag);
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    speedSlider.setMouseDragSensitivity(80);
    addAndMakeVisible(speedLabel);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centred);

    //adjust the sliders' color scheme
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::blue); //dial
    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::greenyellow); //body
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red); //body

    //Include a waveform in every GUI
    addAndMakeVisible(waveformDisplay);

    //insert list of songs that will be played next
    upNext.getHeader().addColumn("Next Song:", 1, 100);
    upNext.setModel(this);
    addAndMakeVisible(upNext);

    // begin calling the tread ten times in a second (every 0.1 seconds).
    startTimer(100);
}
    
DeckGUI::~DeckGUI() 
{
    //end timer when deleting the class
    stopTimer();
}

void DeckGUI::paint(Graphics & g)
{

}

void DeckGUI::resized()
{
    double rowH = getHeight() / 6;
    double colW = getWidth() / 4;

    

    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2);

    posSlider.setBounds(0, rowH * 2, getWidth(), rowH);

    volSlider.setBounds(0, rowH * 3 +20, colW, rowH*3 -30);    
    speedSlider.setBounds(colW, rowH * 3 +20, colW*1.5, rowH*2 - 30);
    upNext.setBounds(colW * 2.5, rowH * 3, colW * 1.5 - 20, rowH * 2);

    playButton.setBounds(colW+10, rowH * 5 + 10, colW-20, rowH-20);
    stopButton.setBounds(colW*2+10, rowH * 5 + 10, colW-20, rowH-20);
    nextButton.setBounds(colW * 3 + 10, rowH * 5 + 10, colW - 20, rowH - 20);

}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        player->start(); //in order to function, load (nextbutton) must be pressed.
    }
    if (button == &stopButton)
    {
        player->stop();
    }
    if (button == &nextButton)
    {   
        //managing the left channel's next button
        if (channel == 0 && playlistComponent->playListL.size() > 0) //only process if new songs are added.
        {
            //get the URL for Left playlist's opening song
            URL fileURL = URL{ File{playlistComponent->playListL[0]} };
            //open the initial URL
            player->loadURL(fileURL);
            // show off the waves
            waveformDisplay.loadURL(fileURL);
            //to prevent it from replaying, pop the Left playlist's initial URL.
            playlistComponent->playListL.erase(playlistComponent->playListL.begin());
        }

        //managing the right channel's next button
        if (channel == 1 && playlistComponent->playListR.size() > 0)
        {
            //get the playlist's first song's URL
            URL fileURL = URL{ File{playlistComponent->playListR[0]} };
            //open the initial URL
            player->loadURL(fileURL);
            // show off the waves
            waveformDisplay.loadURL(fileURL);
            //To prevent it from replaying, pop the first URL of the appropriate playlist.
            playlistComponent->playListR.erase(playlistComponent->playListR.begin()); // eliminate the first element
        }

        //Buttons begin with a load indicator. After the first music loads, we can switch to the next one.
        if (nextButton.getButtonText() == "LOAD")
        {
            nextButton.setButtonText("NEXT");
        }
        else
        {
            player->start(); // launches the player each time the next button is pressed.
        }
    }

    // anytime a button is pressed, the next table is refreshed
    upNext.updateContent();
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        player->setRelativePosition(slider->getValue());
    }

}


int DeckGUI::getNumRows()
{
    //The amount of songs loaded to each channel determines how many rows there are in the table.
    if (channel == 0) //left
    {
        return playlistComponent->playListL.size();
    }
    if (channel == 1) //right
    {
        return playlistComponent->playListR.size();
    }
}

void DeckGUI::paintRowBackground(Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    // adjust each row's background in the following table
    g.fillAll(juce::Colours::mediumspringgreen);
}

void DeckGUI::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    std::string filepath = "";

    // get the file path based on the channel
    if (channel == 0) //left
    {
        filepath = playlistComponent->playListL[rowNumber];
    }
    if (channel == 1) //right
    {
        filepath = playlistComponent->playListR[rowNumber];
    }

    // take the file name out of the path
    std::size_t startFilePos = filepath.find_last_of("\\");
    std::size_t startExtPos = filepath.find_last_of(".");
    std::string extn = filepath.substr(startExtPos + 1, filepath.length() - startExtPos);
    std::string file = filepath.substr(startFilePos + 1, filepath.length() - startFilePos - extn.size() - 2);
    //add a name to every cell
    g.drawText(file,
        1, rowNumber,
        width - 4, height,
        Justification::centredLeft,
        true);
}


void DeckGUI::timerCallback()
{
    waveformDisplay.setRelativePosition(
        player->getRelativePosition());
}
