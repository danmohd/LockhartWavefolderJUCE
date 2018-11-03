/*
  ==============================================================================

    Lockhart.cpp
    Created: 22 May 2018 7:09:18am
    Author:  Danish

  ==============================================================================
*/

#include "Lockhart.h"

double lambertw(double x)
{
	//Upper branch of lambert-W function for non negative inputs.
	//Function adapted from F.Esqueda 2 / 10 / 17 based on implementation presented
	//by Darko Veberic - "Lambert W Function for Applications in Physics"

	jassert(x >= 0);
	double thresh = 10.0e-12;

	//handle 0 separately
	if (x == 0.0) { return 0.0; }

	//initial guesses
	double w = 0.0;
	if (x < 0.14546954290661823)
	{
		double num = 1.0 + (5.931375839364438 + (11.39220550532913 + (7.33888339911111 + 0.653449016991959*x)*x)*x)*x;
		double den = 1.0 + (6.931373689597704 + (16.82349461388016 + (16.43072324143226 + 5.115235195211697*x)*x)*x)*x;
		w = x * num / den;
	}
	else if (x < 8.706658967856612)
	{
		double num = 1.0 + (2.4450530707265568 + (1.3436642259582265 + (0.14844005539759195 + 0.0008047501729130*x)*x)*x)*x;
		double den = 1.0 + (3.4447089864860025 + (3.2924898573719523 + (0.9164600188031222 + 0.05306864044833221*x)*x)*x)*x;
		w = x * num / den;
	}
	else
	{
		double a = std::log(x);
		double b = std::log(a);
		double ia = 1.0 / a;
		w = a - b + (b * ia) + 0.5 * b * (b - 2.0) * (ia * ia) + (1.0 / 6.0) * (2.0 * b * b - 9.0 * b + 6.0) * (ia * ia * ia);
		//Possible typo in Fabian's MATLAB code. His has ... (b * ia) * 0.5 * ... instead of ... (b * ia) + 0.5 * ... in the above line.
	}

	//Fritsch's iterations (limited to 20 iterations or when error e is below the threshold, whichever comes first)
	for (int i = 0; i < 20; ++i)
	{
		double w1 = w + 1.0;
		double z = std::log(x) - std::log(w) - w;
		double q = 2.0 * w1 * (w1 + (2.0 / 3.0) * z);
		double e = (z / w1) * ((q - z) / (q - 2.0 * z));
		w = w * (1.0 + e);

		if (abs(e) < thresh) { break; }
	}

	return w;
}

template <typename T> int sgn(T val)
{
	//signum function
	return (T(0) < val) - (val < T(0));
}

Lockhart::Lockhart()
	: inputPrev(0.0f), integralPrev(0.0f)
{
}

Lockhart::~Lockhart()
{
}

double Lockhart::process(double input, double RL)
{
	//double L = sgn<double>(input);
	//double output = A1 * RL * input - L * iB1 * lambertw(D1 * RL * std::exp(L * (B1 + B2 * RL) * input));
	//return output;

	double output;
	double i1 = integral(input, RL);
	if (std::abs(input - inputPrev) < 10e-3f)
	{
		double L = sgn<double>(input);
		double mInput = 0.5 * (input + inputPrev);
		output = A1 * RL * mInput - L * iB1 * lambertw(D1 * RL * std::exp(L * (B1 + B2 * RL) * mInput));
	}
	else
	{
		output = (i1 - integralPrev) / (input - inputPrev);
	}
	inputPrev = input;
	integralPrev = i1;
	
	return output;
}

double Lockhart::integral(double input, double RL)
{
	double L = sgn<double>(input);
	double psi = lambertw(D1 * RL * std::exp(L * (B1 + B2 * RL) * input));
	double result = 0.5 * A1 * RL * input * input - (n * Vt / (2.0 * (B1 + B2 * RL))) * psi * (psi + 2.0);
	return result;
}