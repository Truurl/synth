//
// Created by Albert on 03.06.2020.
//

#ifndef SYNTH_SAMPLEGENERATION_H
#define SYNTH_SAMPLEGENERATION_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>

// sample rate for generating samples
#define SAMPLE_RATE (48000)

/**
 * @brief   Generates one 16bit sine sample
 * Idea is that all wave samples are generated in same loop, so time increment is done
 * at the end of the loop
 * @param frequency frequency of the wave
 * @param amplitude amplitude of the wave
 * @param time time for which sample is generated
 * @retval  generated sample as int16_t
 */
int16_t GenerateSineSample(double frequency, double amplitude, uint64_t *time);

/**
 * @brief Generates 16bit sample of square wave for given time
 *
 * @param frequency frequency of the wave
 * @param dutyCycle value from 0.0 to 1.0
 * @param amplitude amplitude of the wave
 * @param time time for which sample is generated
 *
 * @retval generated sample as int16_t
 */
int16_t GenerateSquareSample(double frequency, double amplitude, double dutyCycle, uint64_t *time);

/**
 * @brief Generates 16bit sample of triangle wave for given time
 *
 * @param frequency frequency of the wave
 * @param amplitude amplitude of the wave
 * @param time time for which sample is generated
 *
 * @retval generated sample as int16_t
 */
int16_t GenerateTriangleSample(double frequency, double amplitude, uint64_t *time);

/**
 * @brief Generates 16bit sample of sawtooth wave for given time
 *
 * @param frequency frequency of the wave
 * @param amplitude amplitude of the wave
 * @param slope wave can rise to maximum, or start as max than fall 'n' mean negative,
 * 'p' positive
 * @param time time for which sample is generated
 *
 * @retval generated sample as int16_t
 */
int16_t GenerateSawtoothSample(double frequency, double amplitude, char slope, uint64_t *time);

/**
 * @brief looks for maximum value in array
 * @param buffer array with int16_t
 * @param n size of array
 * @retval founded maximum value
 */

int16_t Max(int16_t *buffer, size_t n);


/**
 * @brief Normalize all int16_t to maxValue
 *
 * Uses @see Max() finds maximum value than scales array to 1.0 than to maxValue
 *
 * @param buffer array with int16_t to be normalize
 * @param n size fo array
 * @param maxValue maximum value that int16_t can have after this function
 *
 * @retval nothing
 */

void Normalize(int16_t *buffer, size_t n, int16_t maxValue);

/**
 * @brief Fills array with samples of square root raised cosine
 *
 * Fills with int16_t, but inside all samples are calculated as double than casted to int16_t
 * for faster computing afterward.
 *
 * @param buffer array to be filled with samples
 * @param n size of array
 * @param amplitude scales SRRC to given amplitude
 * @param alfa of SRRC
 * @param delay n samples for which SRRC is delay, shifts to right sustain hold maximum value for longer
 */
void FillFilter(int16_t *buffer, size_t n, double amplitude, double alfa, uint16_t delay, uint16_t sustain);

#endif //SYNTH_SAMPLEGENERATION_H
