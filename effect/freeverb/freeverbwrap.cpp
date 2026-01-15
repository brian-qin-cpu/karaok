#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Freeverb.hpp"
#include "freeverbwrap.h"
#include "convert_format.h"

#define FBUF_SIZE	2048


extern "C" {
	static Freeverb *pInstance = NULL;
	static float *fbuf;

	void freeverb_create(void)
	{
		if (pInstance == NULL) {
			pInstance = new Freeverb();
		}
		fbuf = (float *)malloc(FBUF_SIZE * sizeof(float));
		if (!fbuf) {
			printf("fail to alloc for audio float buffer\n");
		}
		printf("%s: success\n", __func__);
	}

	void freeverb_destroy(void)
	{

		if (pInstance)
			delete pInstance;
		if (fbuf) {
			free(fbuf);
			fbuf = NULL;
		}
	}

	size_t freeverb_process(int16_t *inputs, int16_t *outputs, size_t sampleFrames)
	{
		if (sampleFrames > FBUF_SIZE) {
			printf("%s: out of range, Notice!!!\n", __func__);
			sampleFrames = FBUF_SIZE;
		}
		int16_to_float(fbuf, inputs, sampleFrames);
	
		if (pInstance) {
			//printf("%s: samples = %ld\n", __func__, sampleFrames);
			pInstance->process(&fbuf, &fbuf, sampleFrames);
			return sampleFrames;
		}

		float_to_int16(outputs, fbuf, sampleFrames);

		return 0;			
	}

	size_t freeverb_processReplace(int16_t *inputs, int16_t *outputs, size_t sampleFrames)
	{
		if (sampleFrames > FBUF_SIZE) {
			printf("%s: out of range, Notice!!!\n", __func__);
			sampleFrames = FBUF_SIZE;
		}
		int16_to_float(fbuf, inputs, sampleFrames);

		if (pInstance) {
			pInstance->processReplacing(&fbuf, &fbuf, sampleFrames);
			return sampleFrames;
		}

		float_to_int16(outputs, fbuf, sampleFrames);

		return 0;
	}

	void freeverb_setparameter(long index, float value)
	{
		if (pInstance) {
			pInstance->setParameter(index, value);
		}
	}

	float freeverb_getparameter(long index)
	{
		if (pInstance) {
			return pInstance->getParameter(index);
		}
		return 0.0f;
	}
}
