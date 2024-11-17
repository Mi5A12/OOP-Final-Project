/*
  ==============================================================================
    WaveformDisplay.h
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//With the help of this component, audio waveforms can be painted onto the screen.

class WaveformDisplay  : public juce::Component,
                        public ChangeListener
{
public:

    WaveformDisplay(AudioFormatManager &formatManagerToUse, 
            AudioThumbnailCache &cacheToUse);
    ~WaveformDisplay() override;

    //Customize the input images
    void paint(juce::Graphics&) override;
    //Rescaling of program window components
    void resized() override;

    //Generates the name of the currently playing song and sets the audiothumb's source based on the URL path
    void loadURL(URL audioURL);

    //Listener to redraw upon modification to enable movable playhead
    void changeListenerCallback(ChangeBroadcaster* source) override;

    //Adjust the playhead's relative position
    void setRelativePosition(double pos);

private:

    AudioThumbnail audioThumb;

    bool fileLoaded;
    double position;
    std::string nowPlaying;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
