//
// Created by Albert on 03.06.2020.
//

#include "sampleGeneration.h"



int16_t GetSineSample(double frequency, double amplitude, uint64_t *time)
{
    if( amplitude > 1.0)
    {
        amplitude /= amplitude;
    }
//    *time = (*time + (uint16_t) frequency) % SAMPLE_RATE;
//    return (int16_t) ( (double) sine[*time] * amplitude);
    return (int16_t) ( (double) sine[(*time) * ((uint16_t) frequency)] * amplitude);

}

int16_t GenerateSineSample(double frequency, double amplitude, uint64_t *time)
{
    int16_t sample = sin((double)((2.0 * M_PI * (double)*time * frequency) / SAMPLE_RATE)) * amplitude * INT16_MAX;
    return (int16_t) sample;
}

int16_t GetSquareSample(double frequency, double amplitude, uint64_t *time)
{
    if( amplitude > 1.0)
    {
        amplitude /= amplitude;
    }
    *time = (*time + (uint16_t) frequency) % SAMPLE_RATE;
    return (int16_t) ( (double) square[*time] * amplitude);
}

int16_t GenerateSquareSample(double frequency, double amplitude, double dutyCycle, uint64_t *time)
{

    uint16_t samplesPerCycle = (uint16_t) ((double) SAMPLE_RATE) / frequency;
    uint16_t fallingEdgeSample = (uint16_t) (((double) samplesPerCycle) * dutyCycle) - 1;
    if (fallingEdgeSample < *time )
    {
        return (int16_t) ((double) -1.0 * amplitude * (double) INT16_MAX);
    }
    return (int16_t) ((double) amplitude * (double) INT16_MAX);

}

int16_t GetTriangleSample(double frequency, double amplitude, uint64_t *time)
{
    if( amplitude > 1.0)
    {
        amplitude /= amplitude;
    }
    *time = (*time + (uint16_t) frequency) % SAMPLE_RATE;
    return (int16_t) ( (double) square[*time] * amplitude);
}

int16_t GenerateTriangleSample(double frequency, double amplitude, uint64_t *time)
{

    double sample;
    uint16_t samplesPerCycle = ((double) SAMPLE_RATE) / frequency;
    *time %= samplesPerCycle;
    if( (samplesPerCycle / 4) >= *time)
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

int16_t GetSawtoothSample(double frequency, double amplitude, uint64_t *time)
{
    if( amplitude > 1.0)
    {
        amplitude /= amplitude;
    }
    *time = (*time + (uint16_t) frequency) % SAMPLE_RATE;
    return (int16_t) ( (double) square[*time] * amplitude);
}

int16_t GenerateSawtoothSample(double frequency, double amplitude, char slope, uint64_t *time)
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