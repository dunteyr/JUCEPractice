#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : 
    noisePlayButton("NoisePlayButton", MainComponent::colorPalette.normalColour, MainComponent::colorPalette.overColour, MainComponent::colorPalette.downColour),
    sinPlayButton("SinPlayButton", MainComponent::colorPalette.normalColour, MainComponent::colorPalette.overColour, MainComponent::colorPalette.downColour)
{
    noiseIsPlaying = false;
    sinIsPlaying = false;

    noisePlayButton.setShape(makePlayButtonShape(), true, true, false);
    noisePlayButton.onClick = [this] { onNoisePlayStop(); };

    sinPlayButton.setShape(makePlayButtonShape(), true, true, false);
    sinPlayButton.onClick = [this] { onSinPlayStop(); };

    addAndMakeVisible(noisePlayButton);
    addAndMakeVisible(sinPlayButton);

    addAndMakeVisible(noiseVolLabel);
    noiseVolLabel.setText("Volume", juce::dontSendNotification);
    noiseVolLabel.setJustificationType(juce::Justification::horizontallyCentred);

    addAndMakeVisible(noiseVolSlider);
    noiseVolSlider.setRange(0.0, 1.0, 0.01);
    noiseVolSlider.setColour(juce::Slider::thumbColourId, colorPalette.normalColour);
    noiseVolSlider.onValueChange = [this] {noiseVolume = noiseVolSlider.getValue(); };

    addAndMakeVisible(sinVolSlider);
    sinVolSlider.setRange(0.0, 1.0, 0.01);
    sinVolSlider.setColour(juce::Slider::thumbColourId, colorPalette.normalColour);
    sinVolSlider.onValueChange = [this] { sinVolume = sinVolSlider.getValue(); };
    addAndMakeVisible(sinVolLabel);
    sinVolLabel.setText("Volume", juce::dontSendNotification);
    sinVolLabel.setJustificationType(juce::Justification::horizontallyCentred);

    addAndMakeVisible(sinFreqSlider);
    sinFreqSlider.setRange(50, 10000);
    sinFreqSlider.setValue(440);
    sinFreqSlider.setSkewFactorFromMidPoint(1000.0);
    sinFreqSlider.setNumDecimalPlacesToDisplay(1);
    sinFreqSlider.setColour(juce::Slider::thumbColourId, colorPalette.normalColour);
    sinFreqSlider.onValueChange = [this] { if (sinParams.currentSampleRate > 0.0) updateAngleDelta(); }; //update sine angle delta whenever freq is changed
    addAndMakeVisible(sinFreqLabel);
    sinFreqLabel.setText("Frequency", juce::dontSendNotification);
    sinFreqLabel.setJustificationType(juce::Justification::horizontallyCentred);

    

    
    
    

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

    //sample rate is needed for sine generator
    sinParams.currentSampleRate = sampleRate;
    updateAngleDelta();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    //bufferToFill.clearActiveBufferRegion();

    if (noiseIsPlaying && !sinIsPlaying)
    {
        //for each channel in the buffer, get a pointer to the starting sample (so that it can be written to)
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) 
        {
            auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            //for each sample in the channel's buffer, set it to a random value between -1 and 1 to create a buffer of random noise
            for (auto sample = 0; sample < bufferToFill.numSamples; sample++) {
                float randomValue = random.nextFloat() * 2.0 - 1.0;

                buffer[sample] = randomValue * noiseVolume;
            }
        }
    }

    else if (sinIsPlaying && !noiseIsPlaying)
    {
        //channels have to be filled at the same time with the same value or you get something cooler than a sin wave
        auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (auto sample = 0; sample < bufferToFill.numSamples; sample++)
        {
            auto currentSample = (float)std::sin(sinParams.currentAngle);
            sinParams.currentAngle += sinParams.angleDelta;

            leftBuffer[sample] = currentSample * sinVolume;
            rightBuffer[sample] = currentSample * sinVolume;
        }
            

            

    }

    else if (noiseIsPlaying && sinIsPlaying)
    {

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

    //number of pixels to be used for space between stuff
    int padding = 20;
    int labelWidth = this->getWidth() / 12;
    //the height of an entire row (button, label, slider)
    int rowHeight = this->getHeight() / 10;

    //////////////NOISE VOLUME SLIDER ROW////////////////////////////////////////////
    noisePlayButton.setBounds(0, padding, this->getWidth() / 10, rowHeight);

    //place volume label after the play button
    int volLabelX = noisePlayButton.getBounds().getBottomRight().getX();
    noiseVolLabel.setBounds(volLabelX, padding, this->getWidth() / 12, rowHeight);

    //place slider after label. Take up remaining screen width
    int volSliderWidth = this->getWidth() - noiseVolLabel.getBounds().getWidth() - noisePlayButton.getBounds().getWidth();
    int volSliderX = noiseVolLabel.getBounds().getBottomRight().getX();
    noiseVolSlider.setBounds(volSliderX, padding, volSliderWidth, rowHeight);

    /////////////SINE VOLUME SLIDER ROW//////////////////////////////////////////////
    int sinVolRowYPos = this->getHeight() / 4;
    sinPlayButton.setBounds(0, sinVolRowYPos, this->getWidth() / 10, this->getHeight() / 10);

    sinVolLabel.setBounds(volLabelX, sinVolRowYPos, labelWidth, rowHeight);

    sinVolSlider.setBounds(volSliderX, sinVolRowYPos, volSliderWidth, rowHeight);

    /////////////SINE FREQUENCY SLIDER ROW///////////////////////////////////////////
    int sinFreqRowYPos = this->getHeight() / 8 * 3;
    sinFreqLabel.setBounds(volLabelX, sinFreqRowYPos, labelWidth, rowHeight);

    sinFreqSlider.setBounds(volSliderX, sinFreqRowYPos, volSliderWidth, rowHeight);
}


void MainComponent::updateAngleDelta()
{
    auto cyclesPerSample = sinFreqSlider.getValue() / sinParams.currentSampleRate;         
    sinParams.angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}


juce::Path MainComponent::makePlayButtonShape() 
{
    juce::Path shape;

    shape.addTriangle(0.0, 0.0, 0.0, 4.0, 3.0, 2.0);

    juce::Path roundedShape = shape.createPathWithRoundedCorners(0.5);

    return roundedShape;
}


juce::Path MainComponent::makeStopButtonShape()
{
    juce::Path shape;

    shape.addRectangle(0.0, 0.0, 4.0, 4.0);

    juce::Path roundedShape = shape.createPathWithRoundedCorners(0.5);

    return roundedShape;
}


void MainComponent::onNoisePlayStop()
{
    if (noiseIsPlaying)
    {
        noiseIsPlaying = false;
        noisePlayButton.setShape(makePlayButtonShape(), false, true, false);
    }

    else
    {
        noiseIsPlaying = true;
        noisePlayButton.setShape(makeStopButtonShape(), false, true, false);
    }
}


void MainComponent::onSinPlayStop()
{
    if (sinIsPlaying)
    {
        sinIsPlaying = false;
        sinPlayButton.setShape(makePlayButtonShape(), false, true, false);
    }

    else
    {
        sinIsPlaying = true;
        sinPlayButton.setShape(makeStopButtonShape(), false, true, false);
    }
}
