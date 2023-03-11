#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : noisePlayButton("NoisePlayButton", juce::Colour::Colour(60, 179, 113), juce::Colour::Colour(120, 179, 113), juce::Colour::Colour(180, 179, 113))
{
    noisePlayButton.setShape(makePlayButtonShape(), true, true, false);
    noisePlayButton.setBorderSize(juce::BorderSize<int>(2));

    addAndMakeVisible(noisePlayButton);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(volSlider);
    volSlider.setRange(0.0, 1.0, 0.01);
    volSlider.addListener(this);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.setJustificationType(juce::Justification::horizontallyCentred);
    volLabel.attachToComponent(&volSlider, true);
    
    

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
    //apparently this line isn't needed
    volSlider.removeListener(this);
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    //bufferToFill.clearActiveBufferRegion();

    //for each channel in the buffer, get a pointer to the starting sample (so that it can be written to)
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
        auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        //for each sample in the channel's buffer, set it to a random value between -0.125 and 0.125 to create a buffer of random noise
        for (auto sample = 0; sample < bufferToFill.numSamples; sample++) {
            float randomValue = random.nextFloat() * 2.0 - 1.0;
            
            buffer[sample] = randomValue * noiseVolume;
        }
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    noisePlayButton.setBounds(0, 0, this->getWidth() / 8, this->getHeight() / 10);

    //place volume label after the play button
    int volLabelX = noisePlayButton.getBounds().getBottomRight().getX();
    volLabel.setBounds(volLabelX, 0, this->getWidth() / 12, this->getHeight() / 10);

    //place slider after label. Take up remaining screen width
    int volSliderWidth = this->getWidth() - volLabel.getBounds().getWidth() - noisePlayButton.getBounds().getWidth();
    int volSliderX = volLabel.getBounds().getBottomRight().getX();
    volSlider.setBounds(volSliderX, 0, volSliderWidth, this->getHeight() / 10);
    

}


void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volSlider) {
        noiseVolume = volSlider.getValue();
    }
}


juce::Path MainComponent::makePlayButtonShape() 
{
    juce::Path shape;
    juce::Line<float> line;
    line.setStart(0.0, 0.0);
    line.setEnd(10.0, 0.0);

    shape.addArrow(line, 1, 3, 4);

    return shape;
}
