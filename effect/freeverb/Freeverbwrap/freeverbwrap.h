#ifndef FREEVERB_WRAP_H__
#define FREEVERB_WRAP_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void freeverb_create(void);
void freeverb_destroy(void);
size_t freeverb_process(float **inputs, float **outputs, size_t sampleFrames);
size_t freeverb_processReplace(float **inputs, float **outputs, size_t sampleFrames);
void freeverb_setparameter(long index, float value);
float freeverb_getparameter(long index);

#ifdef __cplusplus
}
#endif

#endif /* FREEVERB_WRAP_H__ */
