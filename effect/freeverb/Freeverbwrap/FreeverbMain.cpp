// Freeverb3 initialisation implementation
// Based on Steinberg VST Development Kit Examples
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include <stdio.h>
#include "Freeverb.hpp"


int main(int argc, char **argv)
{
	Freeverb *mfreeverb = new Freeverb();
	if (!mfreeverb) {
		printf("fail to new freeverb\n");
		return -1;
	}

	return 0;
}



