#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fir_reverb.h"


//#define SAMPLE_RATE		44100
//#define CHANNELS		1



static void generate_reverb_ir(float *ir, size_t length, float rt60, float room_size, int samplerate)
{
	float t60_samples = rt60 * samplerate;
	for (size_t i = 0; i < length; i++) {
		float t = (float) i / samplerate;
		float envelope = expf(-2.302585f * t / rt60); // -2.3022585 = ln(0.1)
		float noise = ((rand() % 20000 - 10000) / 10000.0f) * 0.5f;
		float density_factor = 1.0f + room_size * 4.0f;
		if (i % (int)(50.0f / density_factor) == 0) {
			ir[i] = noise *envelope;
		} else {
			ir[i] = 0.0f;
		}
	}
	float max_val = 0;
	for (size_t i = 0; i < length; i++) {
		if (fabsf(ir[i]) > max_val)
			max_val = fabsf(ir[i]);
	}
	if (max_val > 0) {
		for (size_t i = 0; i < length; i++) {
			ir[i] /= max_val;
		}
	}
}

reverb_t *reverb_create(int samplerate, size_t ir_ms)
{
	reverb_t *rv = (reverb_t *)calloc(1, sizeof(reverb_t));
	if (rv == NULL)
		return NULL;

	rv->ir_length = (size_t)(ir_ms * samplerate / 1000);
	rv->ir_buffer = (short *)malloc(rv->ir_length * sizeof(short));
	rv->delay_line = (float *)calloc(rv->ir_length, sizeof(float));
	if (!rv->ir_buffer || !rv->delay_line) {
		free(rv->ir_buffer);
		free(rv->delay_line);
		free(rv);
		return NULL;
	}

	rv->sample_rate = samplerate;

	rv->params.room_size = 0.6f;
	rv->params.rt60 = 2.0f;
	rv->params.wet_gain = 0.7f;
	rv->params.dry_gain = 1.0f;

	float *temp_ir = (float *)malloc(rv->ir_length * sizeof(float));
	generate_reverb_ir(temp_ir, rv->ir_length, rv->params.rt60, rv->params.room_size, rv->sample_rate);
	for (size_t i = 0; i < rv->ir_length; i++) {
		rv->ir_buffer[i] = (short)(temp_ir[i] * 32767.0f);
	}
	free(temp_ir);
	rv->write_pos = 0;
	printf("%s: finish for fir reverb\n", __func__);

	return rv;
}

void reverb_destroy(reverb_t *rv)
{
	if (rv) {
		free(rv->ir_buffer);
		free(rv->delay_line);
		free(rv);
		rv = NULL;
	}
}

void reverb_set_param(reverb_t *rv, const char *name, float value)
{
	if (strcmp(name, "room_size") == 0) {
		rv->params.room_size = fmaxf(0.1f, fminf(1.0f, value));
	} else if (strcmp(name, "rt60") == 0) {
		rv->params.rt60 = fmaxf(0.1f, fminf(1.0f, value));
	} else if (strcmp(name, "wet_gain") == 0) {
		rv->params.wet_gain = fmaxf(0.1f, fminf(1.0f, value));
	} else if (strcmp(name, "dry_gain") == 0) {
		rv->params.dry_gain = fmaxf(0.1f, fminf(1.0f, value));
	}

	//re generate reverb
	float *temp_ir = (float *)malloc(rv->ir_length * sizeof(float));
	generate_reverb_ir(temp_ir, rv->ir_length, rv->params.rt60, rv->params.room_size, rv->sample_rate);
	for (size_t i = 0; i < rv->ir_length; i++) {
		rv->ir_buffer[i] = (short)(temp_ir[i] * 32767.0f);
	}
	free(temp_ir);
}

short reverb_process(reverb_t *rv, short input)
{
	float dry = input * rv->params.dry_gain;
	float wet = 0.0f;

	rv->delay_line[rv->write_pos] = input;
	//sum(delay_line[i] * ir[(write_pos - i + len) % len])
	size_t pos = rv->write_pos;
	for (size_t i = 0; i < rv->ir_length; i++) {
		size_t idx = (pos + rv->ir_length - i) % rv->ir_length;
		wet += rv->delay_line[idx] * (rv->ir_buffer[i] / 32767.0f);
	}

	rv->write_pos = (rv->write_pos + 1) % rv->ir_length;
	float output = dry + wet * rv->params.wet_gain;

	if (output > 32767.0f) output = 32767.0f;
	if (output < -32768.0f) output = -32768.0f;

	return (short)output;
}

void reverb_process_block(reverb_t *rv, short *in_out, size_t num_samples)
{
	//printf("%s: num_samples=%ld\n", __func__, num_samples);
	for (size_t i = 0; i < num_samples; i++) {
		in_out[i] = reverb_process(rv, in_out[i]);
	}
}
