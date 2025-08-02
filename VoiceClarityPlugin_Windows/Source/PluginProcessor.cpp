#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
VoiceClarityPluginAudioProcessor::VoiceClarityPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr, "Parameters", 
                   {
                       std::make_unique<juce::AudioParameterFloat>("clarity", "Clarity", 0.0f, 10.0f, 5.0f),
                       std::make_unique<juce::AudioParameterFloat>("deesser", "De-esser", 0.0f, 10.0f, 0.0f),
                       std::make_unique<juce::AudioParameterFloat>("highpass", "High Pass", 20.0f, 200.0f, 80.0f),
                       std::make_unique<juce::AudioParameterFloat>("lowpass", "Low Pass", 5000.0f, 20000.0f, 12000.0f),
                       std::make_unique<juce::AudioParameterFloat>("presence", "Presence", 0.0f, 10.0f, 3.0f),
                       std::make_unique<juce::AudioParameterFloat>("warmth", "Warmth", 0.0f, 10.0f, 2.0f)
                   })
{
}

VoiceClarityPluginAudioProcessor::~VoiceClarityPluginAudioProcessor()
{
}

//==============================================================================
const juce::String VoiceClarityPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VoiceClarityPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VoiceClarityPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VoiceClarityPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VoiceClarityPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VoiceClarityPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int VoiceClarityPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VoiceClarityPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String VoiceClarityPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void VoiceClarityPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void VoiceClarityPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Prepare main chain
    mainChain.prepare(spec);

    // Configure high-pass filter (index 0)
    auto& highPassFilter = mainChain.get<0>();
    *highPassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0f);

    // Configure low-pass filter (index 1)
    auto& lowPassFilter = mainChain.get<1>();
    *lowPassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 12000.0f);

    // Prepare de-esser
    deEsser.prepare(spec);
    deEsser.setThreshold(-20.0f);
    deEsser.setRatio(4.0f);
    deEsser.setAttack(1.0f);
    deEsser.setRelease(50.0f);

    // Prepare clarity gain
    clarityGain.prepare(spec);
}

void VoiceClarityPluginAudioProcessor::releaseResources()
{
    mainChain.reset();
    deEsser.reset();
    clarityGain.reset();
}

bool VoiceClarityPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void VoiceClarityPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameter values
    auto clarityValue = parameters.getRawParameterValue("clarity")->load();
    auto deesserValue = parameters.getRawParameterValue("deesser")->load();
    auto highpassValue = parameters.getRawParameterValue("highpass")->load();
    auto lowpassValue = parameters.getRawParameterValue("lowpass")->load();
    auto presenceValue = parameters.getRawParameterValue("presence")->load();
    auto warmthValue = parameters.getRawParameterValue("warmth")->load();

    // Update filter coefficients
    auto& highPassFilter = mainChain.get<0>();
    *highPassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), highpassValue);

    auto& lowPassFilter = mainChain.get<1>();
    *lowPassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lowpassValue);

    // Create audio block
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    // Apply main chain (filters)
    mainChain.process(context);

    // Apply de-esser if enabled
    if (deesserValue > 0.0f)
    {
        // Simple de-esser implementation using frequency-specific compression
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float inputSample = channelData[sample];
                
                // Simple high-frequency detection and attenuation
                static float previousSample = 0.0f;
                float highFreqContent = std::abs(inputSample - previousSample);
                
                if (highFreqContent > 0.1f * (deesserValue / 10.0f))
                {
                    channelData[sample] *= (1.0f - (deesserValue / 20.0f));
                }
                
                previousSample = inputSample;
            }
        }
    }

    // Apply clarity enhancement
    if (clarityValue > 0.0f)
    {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                float inputSample = channelData[sample];
                
                // Enhance mid-high frequencies for clarity
                float enhancedSample = inputSample;
                
                // Simple presence boost (2-5kHz range simulation)
                enhancedSample += inputSample * 0.1f * (presenceValue / 10.0f);
                
                // Warmth (low-mid frequencies)
                enhancedSample += inputSample * 0.05f * (warmthValue / 10.0f);
                
                // Overall clarity gain
                enhancedSample *= (1.0f + (clarityValue / 20.0f));
                
                channelData[sample] = enhancedSample;
            }
        }
    }

    // Apply soft limiting to prevent clipping
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = std::tanh(channelData[sample]);
        }
    }
}

//==============================================================================
bool VoiceClarityPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VoiceClarityPluginAudioProcessor::createEditor()
{
    return new VoiceClarityPluginAudioProcessorEditor (*this);
}

//==============================================================================
void VoiceClarityPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VoiceClarityPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VoiceClarityPluginAudioProcessor();
}

