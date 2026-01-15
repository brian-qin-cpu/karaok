#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "convert_format.h"


void int16_to_float(float *output, const int16_t *input, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		output[i] = input[i] * (1 / 32768.0f);
	}
}

void float_to_int16(int16_t *output, const float *input, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		float tempf = input[i] * 32768.0f;
		if (tempf > 32767.0f) tempf = 32767.0f;
		if (tempf < -32768.0f) tempf = -32768.0f;
		output[i] = (int16_t)roundf(tempf);
	}
}
