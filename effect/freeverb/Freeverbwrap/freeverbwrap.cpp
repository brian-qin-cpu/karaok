#include "Freeverb.hpp"

extern "C" {
	static Freeverb *pInstance = NULL;

	void freeverb_create(void)
	{
		if (pInstance == NULL) {
			pInstance = new Freeverb();
		}
	}

	void freeverb_destroy(void)
	{
		if (pInstance)
			delete pInstance;
	}

	size_t freeverb_process(float **inputs, float **outputs, size_t sampleFrames)
	{
		if (pInstance) {
			pInstance->process(inputs, outputs, sampleFrames);
			return sampleFrames;
		}
		return 0;			
	}

	size_t freeverb_processReplace(float **inputs, float **outputs, size_t sampleFrames)
	{
		if (pInstance) {
			pInstance->processReplacing(inputs, outputs, sampleFrames);
			return sampleFrames;
		}
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
		return 0.0f
	}
}
