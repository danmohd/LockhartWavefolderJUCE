/*
  ==============================================================================

    Lockhart.h
    Created: 22 May 2018 7:09:18am
    Author:  Danish

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Lockhart
{
public:
	Lockhart();
	~Lockhart();
	double process(double input, double RL);
	double integral(double input, double RL);

private:
	const double R = 15.0e3;
	const double Is = 1.0e-17;
	const double Vt = 0.025864;
	const double Af = 1.0;
	const double n = 1.0;
	const double iB1 = n * Vt;
	const double B1 = 1.0 / iB1;
	const double B2 = 2.0 * Af * B1 / R;
	const double D1 = Is * B1;
	const double A1 = 2.0 * Af / R;

	double integralPrev;
	double inputPrev;
};


