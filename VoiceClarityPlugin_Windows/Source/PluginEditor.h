#pragma once

#include "PluginProcessor.h"

//==============================================================================
class VoiceClarityPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit VoiceClarityPluginAudioProcessorEditor (VoiceClarityPluginAudioProcessor&);
    ~VoiceClarityPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    VoiceClarityPluginAudioProcessor& processorRef;

    // Sliders
    juce::Slider claritySlider;
    juce::Slider deesserSlider;
    juce::Slider highpassSlider;
    juce::Slider lowpassSlider;
    juce::Slider presenceSlider;
    juce::Slider warmthSlider;

    // Labels
    juce::Label clarityLabel;
    juce::Label deesserLabel;
    juce::Label highpassLabel;
    juce::Label lowpassLabel;
    juce::Label presenceLabel;
    juce::Label warmthLabel;
    juce::Label titleLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clarityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> deesserAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highpassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowpassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> presenceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> warmthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceClarityPluginAudioProcessorEditor)
};

