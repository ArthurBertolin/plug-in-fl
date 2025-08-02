#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VoiceClarityPluginAudioProcessorEditor::VoiceClarityPluginAudioProcessorEditor (VoiceClarityPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setSize (600, 400);

    // Title Label
    titleLabel.setText("Voice Clarity Plugin", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // Setup Clarity Slider
    claritySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    claritySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    claritySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
    claritySlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(claritySlider);

    clarityLabel.setText("Clarity", juce::dontSendNotification);
    clarityLabel.setJustificationType(juce::Justification::centred);
    clarityLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(clarityLabel);

    clarityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "clarity", claritySlider);

    // Setup De-esser Slider
    deesserSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    deesserSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    deesserSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    deesserSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(deesserSlider);

    deesserLabel.setText("De-esser", juce::dontSendNotification);
    deesserLabel.setJustificationType(juce::Justification::centred);
    deesserLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(deesserLabel);

    deesserAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "deesser", deesserSlider);

    // Setup High Pass Filter Slider
    highpassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highpassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    highpassSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::green);
    highpassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(highpassSlider);

    highpassLabel.setText("High Pass", juce::dontSendNotification);
    highpassLabel.setJustificationType(juce::Justification::centred);
    highpassLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(highpassLabel);

    highpassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "highpass", highpassSlider);

    // Setup Low Pass Filter Slider
    lowpassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lowpassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lowpassSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
    lowpassSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(lowpassSlider);

    lowpassLabel.setText("Low Pass", juce::dontSendNotification);
    lowpassLabel.setJustificationType(juce::Justification::centred);
    lowpassLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(lowpassLabel);

    lowpassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "lowpass", lowpassSlider);

    // Setup Presence Slider
    presenceSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    presenceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    presenceSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::yellow);
    presenceSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(presenceSlider);

    presenceLabel.setText("Presence", juce::dontSendNotification);
    presenceLabel.setJustificationType(juce::Justification::centred);
    presenceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(presenceLabel);

    presenceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "presence", presenceSlider);

    // Setup Warmth Slider
    warmthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    warmthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    warmthSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::purple);
    warmthSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(warmthSlider);

    warmthLabel.setText("Warmth", juce::dontSendNotification);
    warmthLabel.setJustificationType(juce::Justification::centred);
    warmthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(warmthLabel);

    warmthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "warmth", warmthSlider);
}

VoiceClarityPluginAudioProcessorEditor::~VoiceClarityPluginAudioProcessorEditor()
{
}

//==============================================================================
void VoiceClarityPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background gradient
    g.fillAll(juce::Colour::fromRGB(30, 30, 40));
    
    juce::ColourGradient gradient(juce::Colour::fromRGB(40, 40, 50), 0, 0,
                                  juce::Colour::fromRGB(20, 20, 30), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Border
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 2);
}

void VoiceClarityPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Title
    titleLabel.setBounds(bounds.removeFromTop(60).reduced(10));
    
    // Create grid for controls
    auto controlArea = bounds.reduced(20);
    auto sliderWidth = 120;
    auto sliderHeight = 120;
    auto labelHeight = 25;
    
    // Top row
    auto topRow = controlArea.removeFromTop(sliderHeight + labelHeight + 10);
    
    // Clarity
    auto clarityArea = topRow.removeFromLeft(sliderWidth);
    claritySlider.setBounds(clarityArea.removeFromTop(sliderHeight));
    clarityLabel.setBounds(clarityArea.removeFromTop(labelHeight));
    
    topRow.removeFromLeft(20); // spacing
    
    // De-esser
    auto deesserArea = topRow.removeFromLeft(sliderWidth);
    deesserSlider.setBounds(deesserArea.removeFromTop(sliderHeight));
    deesserLabel.setBounds(deesserArea.removeFromTop(labelHeight));
    
    topRow.removeFromLeft(20); // spacing
    
    // Presence
    auto presenceArea = topRow.removeFromLeft(sliderWidth);
    presenceSlider.setBounds(presenceArea.removeFromTop(sliderHeight));
    presenceLabel.setBounds(presenceArea.removeFromTop(labelHeight));
    
    controlArea.removeFromTop(20); // spacing between rows
    
    // Bottom row
    auto bottomRow = controlArea.removeFromTop(sliderHeight + labelHeight + 10);
    
    // High Pass
    auto highpassArea = bottomRow.removeFromLeft(sliderWidth);
    highpassSlider.setBounds(highpassArea.removeFromTop(sliderHeight));
    highpassLabel.setBounds(highpassArea.removeFromTop(labelHeight));
    
    bottomRow.removeFromLeft(20); // spacing
    
    // Low Pass
    auto lowpassArea = bottomRow.removeFromLeft(sliderWidth);
    lowpassSlider.setBounds(lowpassArea.removeFromTop(sliderHeight));
    lowpassLabel.setBounds(lowpassArea.removeFromTop(labelHeight));
    
    bottomRow.removeFromLeft(20); // spacing
    
    // Warmth
    auto warmthArea = bottomRow.removeFromLeft(sliderWidth);
    warmthSlider.setBounds(warmthArea.removeFromTop(sliderHeight));
    warmthLabel.setBounds(warmthArea.removeFromTop(labelHeight));
}

