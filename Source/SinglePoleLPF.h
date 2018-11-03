/*
  ==============================================================================

    SinglePoleLPF.h
    Created: 7 Jun 2018 6:57:08pm
    Author:  Danish

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SinglePoleLPF
{
public:
	enum Unit
	{
		MS,
		HZ
	};
	SinglePoleLPF(double param, Unit unit, double sampleRate);
	~SinglePoleLPF();

	double process(double x0);
private:
	double b0;
	double b1;
	double a0;
	double a1;

	double x1;
	double y1;
};