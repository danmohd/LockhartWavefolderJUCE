/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavefolderAudioProcessor::WavefolderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	addParameter(
		RL = new AudioParameterFloat(
		"RL",
		"Load Resistance",
		1.0e3,
		50.0e3,
		25.0e3
		)
	);
	addParameter(
		Gi = new AudioParameterFloat(
			"Gi",
			"Input Gain",
			0.0,
			10.0,
			1.0
		)
	);
	addParameter(
		V0 = new AudioParameterFloat(
			"V0",
			"DC Offset",
			-1.0,
			1.0,
			0.0
		)
	);
}

WavefolderAudioProcessor::~WavefolderAudioProcessor()
{
}

//==============================================================================
const String WavefolderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavefolderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavefolderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavefolderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavefolderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavefolderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavefolderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavefolderAudioProcessor::setCurrentProgram (int index)
{
}

const String WavefolderAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavefolderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void WavefolderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	samples = samplesPerBlock;
	LoadResistanceSmoother = std::make_unique<SinglePoleLPF>(200.0, SinglePoleLPF::MS, sampleRate);
	InputGainSmoother = std::make_unique<SinglePoleLPF>(200.0, SinglePoleLPF::MS, sampleRate);
	DCOffsetSmoother = std::make_unique<SinglePoleLPF>(200.0, SinglePoleLPF::MS, sampleRate);
}

void WavefolderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavefolderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WavefolderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
	auto* channelDataL = buffer.getWritePointer(0);
	auto* channelDataR = buffer.getWritePointer(1);

	for (auto i = 0; i < samples; ++i)
	{
		double RLSmoothed = LoadResistanceSmoother->process(*RL);
		double GiSmoothed = InputGainSmoother->process(*Gi);
		double V0Smoothed = DCOffsetSmoother->process(*V0);
		
		//Left channel
		double tempL = (V0Smoothed + GiSmoothed * (double)channelDataL[i]);
		tempL = lockharts[0].process(tempL, RLSmoothed);
		tempL = lockharts[1].process(tempL, RLSmoothed);
		channelDataL[i] = (float)lockharts[2].process(tempL, RLSmoothed);
		//Right channel
		double tempR = (V0Smoothed + GiSmoothed * (double)channelDataR[i]);
		tempR = lockharts[3].process(tempR, RLSmoothed);
		tempR = lockharts[4].process(tempR, RLSmoothed);
		channelDataR[i] = (float)lockharts[5].process(tempR, RLSmoothed);
	}
}

//==============================================================================
bool WavefolderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* WavefolderAudioProcessor::createEditor()
{
    return new WavefolderAudioProcessorEditor (*this);
}

//==============================================================================
void WavefolderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	std::unique_ptr<XmlElement> xml(new XmlElement("Wavefolder"));
	xml->setAttribute("RL", (double)*RL);
	xml->setAttribute("Gi", (double)*Gi);
	xml->setAttribute("V0", (double)*V0);
	copyXmlToBinary(*xml, destData);
}

void WavefolderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName("Wavefolder"))
		{
			*RL = (float)xmlState->getDoubleAttribute("RL", 1000.0);
			*Gi = (float)xmlState->getDoubleAttribute("Gi", 1.0);
			*V0 = (float)xmlState->getDoubleAttribute("V0", 0.0);
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavefolderAudioProcessor();
}
