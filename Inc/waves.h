//
// Created by Albert on 25.04.2020.
//

#ifndef WAVES_H
#define WAVES_H

#define SAMPLE_RATE 44100

int16_t GenerateSineSample(double frequency, double amplitude, uint16_t *time);

int16_t GenerateSquareSample(double frequency, double amplitude, double dutyCycle, uint16_t *time);

int16_t GenerateTriangleSample(double frequency, double amplitude, uint16_t *time);

int16_t GenerateSawtoothSample(double frequency, double amplitude, char slope, uint16_t *time);

#endif //WAVES_H
