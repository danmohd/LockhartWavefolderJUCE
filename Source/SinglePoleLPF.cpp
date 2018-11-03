/*
  ==============================================================================

    SinglePoleLPF.cpp
    Created: 7 Jun 2018 6:57:08pm
    Author:  Danish

  ==============================================================================
*/

#include "SinglePoleLPF.h"

SinglePoleLPF::SinglePoleLPF(double param, Unit unit, double sampleRate)
	: x1(0), y1(0)
{
	switch (unit)
	{
		case MS:
		{
			double tr = param / 1000.0;
			a0 = std::tan(float_Pi / (sampleRate * tr)) + 1.0f;
			a1 = a0 - 2.0f;
			b0 = a0 - 1.0f;
			b1 = b0;
			break;
		}
		case HZ:
		{
			a0 = std::tan(float_Pi * param / sampleRate) + 1.0f;
			a1 = a0 - 2.0f;
			b0 = a0 - 1.0f;
			b1 = b0;
			break;
		}
	}
}

SinglePoleLPF::~SinglePoleLPF()
{
}

double SinglePoleLPF::process(double x0)
{
	//Direct Form I implementation
	double y0 = (b0/a0) * x0 + (b1/a0) * x1 - (a1/a0) * y1;
	x1 = x0;
	y1 = y0;
	return y0;
}