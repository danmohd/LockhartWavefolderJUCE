/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WavefolderAudioProcessorEditor  : public AudioProcessorEditor,
	private Timer
{
public:
	WavefolderAudioProcessorEditor (WavefolderAudioProcessor&);
    ~WavefolderAudioProcessorEditor();

    //==============================================================================
	enum
	{
		paramSliderSize = 100,
		paramLabelHeight = 40,
		paramLabelWidth = 100,
		margin = 10
	};
	
	void paint (Graphics&) override;
    void resized() override;
	void changeSliderValue(Slider* slider);
	void startDragChange(Slider* slider);
	void endDragChange(Slider* slider);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavefolderAudioProcessor& processor;

	OwnedArray<Slider> paramSliders;
	OwnedArray<Label> paramLabels;
	Label noParameterLabel { "noparam", "No Parameters Available" };
	void timerCallback() override;
	AudioParameterFloat* getParameterForSlider(Slider* slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavefolderAudioProcessorEditor)
};
