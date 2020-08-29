//
// Created by Albert on 03.06.2020.
//

#ifndef SYNTH_SAMPLEGENERATION_H
#define SYNTH_SAMPLEGENERATION_H

#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <waves.h>
#include "uart.h"
#include "stdio.h"
#include "delay.h"

#define SAMPLE_RATE (48000)

int16_t GetSineSample(double frequency, double amplitude, uint64_t *time);

int16_t GenerateSineSample(double frequency, double amplitude, uint64_t *time);

int16_t GetSquareSample(double frequency, double amplitude, uint64_t *time);

int16_t GenerateSquareSample(double frequency, double amplitude, double dutyCycle, uint64_t *time);

int16_t GetTriangleSample(double frequency, double amplitude, uint64_t *time);

int16_t GenerateTriangleSample(double frequency, double amplitude, uint64_t *time);

int16_t GetSawtoothSample(double frequency, double amplitude, uint64_t *time);

int16_t GenerateSawtoothSample(double frequency, double amplitude, char slope, uint64_t *time);

#endif //SYNTH_SAMPLEGENERATION_H
