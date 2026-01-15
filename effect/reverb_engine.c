#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reverb_engine.h"

enum
{
	KMode, KRoomSize, KDamp, KWidth, KWet, KDry,
	KNumParams
};

static int active_reverb = FREE_REVERB;
static reverb_t *pReverb;

void reverb_engine_create(REVERB_TYPE_EN type, int rate)
{
	active_reverb = type;
	switch (type) {
		case FREE_REVERB:
			freeverb_create();
			break;
		case FIR_REVERB:
			pReverb = reverb_create(rate, 10);//10ms
			break;
		default:
			printf("wrong reverb type %d\n", type);
			break;
	}
}

void reverb_engine_destroy(void)
{
	REVERB_TYPE_EN type = (REVERB_TYPE_EN)active_reverb;

	switch(type) {
		case FREE_REVERB:
			freeverb_destroy();
			break;
		case FIR_REVERB:
			reverb_destroy(pReverb);
			break;
		default:
			printf("wrong type %d\n", type);
			break;
	}
}

void reverb_engine_set_params(float roomsize, float rt60, float wetgain, float drygain)
{
	REVERB_TYPE_EN type = (REVERB_TYPE_EN)active_reverb;

	switch(type) {
		case FREE_REVERB:
			freeverb_setparameter(KRoomSize, roomsize);
			freeverb_setparameter(KDamp, 0.5f);
			freeverb_setparameter(KWet, wetgain);
			freeverb_setparameter(KDry, drygain);
			break;
		case FIR_REVERB:
			reverb_set_param(pReverb, "rt60", rt60);
			reverb_set_param(pReverb, "room_size", roomsize);
			reverb_set_param(pReverb, "wet_gain", wetgain);
			reverb_set_param(pReverb, "dry_gain", drygain);
			break;
		default:
			printf("wrong type %d\n", type);
			break;
	}
}

size_t reverb_engine_process(void *buffer, size_t samples)
{
	size_t ret = 0;
	REVERB_TYPE_EN type = (REVERB_TYPE_EN)active_reverb;

	switch(type) {
		case FREE_REVERB:
			ret = freeverb_process(buffer, buffer, samples);
			break;
		case FIR_REVERB:
			reverb_process_block(pReverb, buffer, samples);
			ret = samples;
			break;
		default:
			printf("wrong type %d\n", type);
			break;
	}
	return ret;
}

