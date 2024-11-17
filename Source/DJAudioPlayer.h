/*
  ==============================================================================
    DJAudioPlayer.h
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

    //This component controls the actual playing of the audio files

class DJAudioPlayer : public AudioSource
{
public:

    DJAudioPlayer(AudioFormatManager& formatManager);
    ~DJAudioPlayer();

    //Pure virtual AudioSource override.
    //To instruct the Audio Source component to get ready to play
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    //Pure virtual AudioSource override.
    //In order for the Audio Source component to obtain the ensuing audio data blocks
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    //Pure virtual AudioSource override.
    //To release everything that the Audio Source component is no longer in need of
    void releaseResources() override;


    //Use URL path to set source for the reader and transport*/
    void loadURL(URL audioURL);
    //Adjust gain (volume) according to the slider's input value, which ranges from 0 to 1.
    void setGain(double gain);
    //Set speed as a ratio of speed based on input value; default speed is 1x.
    void setSpeed(double ratio);
    //Determines location in seconds by using the input value (0–1) that is obtained from the playback slider.
    void setRelativePosition(double pos);
    //Adjust the transport source playhead's location to the input value in seconds
    void setPosition(double posInSecs);

    //Plot the playhead on the waveform by retrieving the playhead's relative position and returning a value
    double getRelativePosition();

    //Start the transport source
    void start();
    //Stop the transport source
    void stop();


private:
    
    AudioFormatManager& formatManager;

    std::unique_ptr<AudioFormatReaderSource> readerSource;

    AudioTransportSource transportSource;

    ResamplingAudioSource resampleSource{ &transportSource, false, 2 };

};
