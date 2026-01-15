#ifndef REVERB_ENGINE_H__
#define REVERB_ENGINE_H__

#include "fir_reverb.h"
#include "freeverbwrap.h"

typedef enum {
	FREE_REVERB,
	FIR_REVERB
} REVERB_TYPE_EN;

void reverb_engine_create(REVERB_TYPE_EN type, int rate);
void reverb_engine_destroy(void);
void reverb_engine_set_params(float roomsize, float rt60, float wetgain, float drygain);
size_t reverb_engine_process(void *buffer, size_t samples);

#endif /* REVERB_ENGINE_H__ */
