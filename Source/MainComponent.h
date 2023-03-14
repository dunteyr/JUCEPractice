#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    struct ColorPalette 
    {
        juce::Colour normalColour = juce::Colour::Colour(60, 179, 113);
        juce::Colour overColour = juce::Colour::Colour(40, 159, 93);
        juce::Colour downColour = juce::Colour::Colour(20, 139, 73);

    }colorPalette;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

private:

    struct SinParams
    {
        double currentSampleRate = 0.0, currentAngle = 0.0, angleDelta = 0.0;
    }sinParams;

    juce::Path makePlayButtonShape();
    juce::Path makeStopButtonShape();
    void onNoisePlayStop();
    void onSinPlayStop();

    void updateAngleDelta();
    //==============================================================================
    // Your private member variables go here...
    bool noiseIsPlaying;
    bool sinIsPlaying;

    float noiseVolume = 0.0;

    juce::Random random;

    juce::Slider noiseVolSlider;
    juce::Label noiseVolLabel;

    juce::Slider sinVolSlider;
    juce::Label sinVolLabel;

    juce::Slider sinFreqSlider;
    juce::Label sinFreqLabel;

    juce::ShapeButton noisePlayButton;
    juce::ShapeButton sinPlayButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
