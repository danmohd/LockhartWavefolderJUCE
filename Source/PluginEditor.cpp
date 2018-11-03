/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavefolderAudioProcessorEditor::WavefolderAudioProcessorEditor (WavefolderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

	auto& params = processor.getParameters();
	for (auto p : params)
	{
		if (auto* param = dynamic_cast<AudioParameterFloat*>(p))
		{
			Slider* aSlider;
			paramSliders.add(aSlider = new Slider(param->name));
			aSlider->setRange(param->range.start, param->range.end);
			aSlider->setSliderStyle(Slider::RotaryVerticalDrag);
			aSlider->setValue(*param);
			aSlider->onValueChange = [this, aSlider] { changeSliderValue(aSlider); };
			aSlider->onDragStart = [this, aSlider] { startDragChange(aSlider); };
			aSlider->onDragEnd = [this, aSlider] { endDragChange(aSlider); };
			aSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, paramSliderSize, paramSliderSize / 5);
			addAndMakeVisible(aSlider);

			Label* aLabel;
			paramLabels.add(aLabel = new Label(param->name, param->name));
			aLabel->setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
			addAndMakeVisible(aLabel);
		}
	}

	noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
	noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));

	if (paramSliders.size() == 0)
	{
		addAndMakeVisible(noParameterLabel);
		setSize(300, 100);
	}
	else
	{
		setSize(
			paramSliderSize + 2 * margin,
			jmax(100, (paramSliderSize + paramLabelHeight) * paramSliders.size())
		);
		startTimer(100);
	}
}

WavefolderAudioProcessorEditor::~WavefolderAudioProcessorEditor()
{
}

//==============================================================================
void WavefolderAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void WavefolderAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	auto r = getLocalBounds();
	noParameterLabel.setBounds(r);

	r.removeFromLeft(margin);
	r.removeFromRight(margin);
	for (auto i = 0; i < paramSliders.size(); ++i)
	{
		auto paramBounds = r.removeFromTop(paramSliderSize);
		auto labelBounds = r.removeFromTop(paramLabelHeight);

		paramLabels[i]->setBounds(labelBounds);
		paramSliders[i]->setBounds(paramBounds);
	}
}

void WavefolderAudioProcessorEditor::changeSliderValue(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		*param = (float)slider->getValue();
}

void WavefolderAudioProcessorEditor::startDragChange(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		param->beginChangeGesture();
}

void WavefolderAudioProcessorEditor::endDragChange(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		param->endChangeGesture();
}

void WavefolderAudioProcessorEditor::timerCallback()
{
	auto& params = processor.getParameters();
	for (auto i = 0; i < params.size(); ++i)
	{
		if (auto* param = dynamic_cast<AudioParameterFloat*> (params[i]))
		{
			if (i < paramSliders.size()) { paramSliders[i]->setValue(*param); }
		}
	}
}

AudioParameterFloat* WavefolderAudioProcessorEditor::getParameterForSlider(Slider * slider)
{
	auto& params = processor.getParameters();
	return dynamic_cast<AudioParameterFloat*>(params[paramSliders.indexOf(slider)]);
}
