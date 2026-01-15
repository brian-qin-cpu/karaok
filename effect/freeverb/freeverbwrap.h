#ifndef FREEVERB_WRAP_H__
#define FREEVERB_WRAP_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short int16_t;

void freeverb_create(void);
void freeverb_destroy(void);
size_t freeverb_process(int16_t *inputs, int16_t *outputs, size_t sampleFrames);
size_t freeverb_processReplace(int16_t *inputs, int16_t *outputs, size_t sampleFrames);
void freeverb_setparameter(long index, float value);
float freeverb_getparameter(long index);

#ifdef __cplusplus
}
#endif

#endif /* FREEVERB_WRAP_H__ */
