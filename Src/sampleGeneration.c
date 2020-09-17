//
// Created by Albert on 03.06.2020.
//
#include "sampleGeneration.h"

int16_t GenerateSineSample(double frequency, double amplitude, uint64_t *time)
{
    int16_t sample = sin((double)((2.0 * M_PI * (double)*time * frequency) / SAMPLE_RATE)) * amplitude * INT16_MAX;
    return (int16_t) sample;
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

int16_t Max(int16_t *buffer, size_t n)
{
    int16_t max = *buffer;

    for(size_t index = 0; index < n; ++index)
    {
        if(max <= *(buffer + index))
        {
            max = *(buffer + index);
        }
    }
    return max;
}

void Normalize(int16_t *buffer, size_t n, int16_t maxValue)
{
    int16_t maximum = Max(buffer, n);

    for(uint16_t index = 0; index < n; ++index)
    {
        *(buffer + index) =  (int16_t)  ((( (double) *(buffer + index) )  / maximum) * maxValue);
    }
}

void FillFilter(int16_t *buffer, size_t n, double amplitude, double alfa, uint16_t delay, uint16_t sustain)
{
    double Ts = 2.0/n;
    double dt = 1.0/SAMPLE_RATE;
    uint16_t t = 0;

    for (uint16_t index = 0; index<n; ++index)
    {

        if ( (delay-index) == 0)
        {
            *(buffer+index) = (int16_t) round(amplitude*(1.0-alfa)+(4.0*alfa)/M_PI);
            // makes maximum value for longer
            for (t; t<sustain; ++t, ++index)
            {
                *(buffer+index) = (int16_t) round(amplitude*(1.0-alfa)+(4.0*alfa)/M_PI);
            }
        }
        else if (isnan(amplitude*(sin(M_PI*(1.0-alfa)*((delay-index+t)*dt/Ts))+
                        4.0*alfa*((delay-index+t)*dt/Ts)*cos(M_PI*(1.0+alfa)*((delay-index+t)*dt/Ts)))/
                        ((M_PI*((delay-index+t)*dt/Ts))*(1-pow(4*alfa*((delay-index+t)*dt/Ts), 2.0)))))
        {
            *(buffer+index) = round(amplitude*(alfa/sqrt(2.0))
                    *((1+(2.0/M_PI))*sin(M_PI/(4.0*alfa))+(1-(2.0/M_PI))*cos(M_PI/(4.0*alfa))));
        }
        else
        {
            *(buffer+index) = round(amplitude*(sin(M_PI*(1.0-alfa)*((delay-index+t)*dt/Ts))+
                    4.0*alfa*((delay-index+t)*dt/Ts)*cos(M_PI*(1.0+alfa)*((delay-index+t)*dt/Ts)))/
                    ((M_PI*((delay-index+t)*dt/Ts))*(1-pow(4*alfa*((delay-index+t)*dt/Ts), 2.0))));
        }
        if ( *(buffer+index) <= 0)
        {
            *(buffer+index) = 0;
        }
    }

}
