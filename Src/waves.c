//
// Created by Albert on 25.04.2020.
//
#include "stdint.h"
#include "waves.h"
#include "math.h"
#include "limits.h"

#define SAMPLE_RATE 44100

int16_t GenerateSineSample(double frequency, double amplitude, uint16_t *time)
{
	uint16_t samplesPerCycle = ((double) SAMPLE_RATE / frequency) ;
	*time %= samplesPerCycle;
	double sample = sin((double)((2.0 * M_PI * (double)*time * frequency) / SAMPLE_RATE)) * amplitude * INT16_MAX;
	(*time)++;
	return (int16_t) sample;
}

int16_t GenerateSquareSample(double frequency, double amplitude, double dutyCycle, uint16_t *time)
{

	uint16_t samplesPerCycle = ((double) SAMPLE_RATE) / frequency;
	uint16_t fallingEdgeSample = (((double) samplesPerCycle) * dutyCycle) - 1;
	*time %= samplesPerCycle;
	if (fallingEdgeSample < *time )
	{
		(*time)++;
		return (int16_t) ((double) -1 * amplitude * (double) INT16_MAX);
	}
	(*time)++;
	return (int16_t) ((double) amplitude * (double) INT16_MAX);

}

int16_t GenerateTriangleSample(double frequency, double amplitude, uint16_t *time)
{

	double sample;
	uint16_t samplesPerCycle = ((double) SAMPLE_RATE) / frequency;
	*time %= samplesPerCycle;
	if( (samplesPerCycle / 4) > *time)
	{
		sample =  ((4.0 * amplitude ) / samplesPerCycle) * (*time) * INT16_MAX;
		(*time)++;
		return (int16_t) sample;
	}

	if(((samplesPerCycle * 3/4) <= *time) && ((samplesPerCycle / 4) < *time))
	{
		sample =  (((4.0 * amplitude ) / samplesPerCycle) * (*time)  - (4 * amplitude)) * INT16_MAX;
		(*time)++;
		return (int16_t) sample;
	}


	if((samplesPerCycle / 4) <= *time)
	{
		sample =  (((-4.0 * amplitude ) / (double)samplesPerCycle) * (*time) + (2 * amplitude)) * INT16_MAX;
		(*time)++;
		return (int16_t) sample;
	}


}

int16_t GenerateSawtoothSample(double frequency, double amplitude, char slope, uint16_t *time)
{

	uint16_t samplesPerCycle = ((double) SAMPLE_RATE / frequency) ;
	*time %= samplesPerCycle;
	if(slope == 'n')
	{
		(*time)++;
		return (int16_t) ((double) (((-2.0 * amplitude) / samplesPerCycle) * (*time) + amplitude) * INT16_MAX);
	}
	(*time)++;
	return (int16_t) ((double) (((2.0 * amplitude) / samplesPerCycle) * (*time) - amplitude) * INT16_MAX);

}