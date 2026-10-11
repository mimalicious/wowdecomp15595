// Reverb model implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "revmodel.h"

// OFFSET: 0x00AD7520/0x1408A27B0
revmodel::revmodel()
{
	// Tie the components to their buffers
	combL[0].setbuffer(bufcombL1,combtuningL1);
	combR[0].setbuffer(bufcombR1,combtuningR1);
	combL[1].setbuffer(bufcombL2,combtuningL2);
	combR[1].setbuffer(bufcombR2,combtuningR2);
	combL[2].setbuffer(bufcombL3,combtuningL3);
	combR[2].setbuffer(bufcombR3,combtuningR3);
	combL[3].setbuffer(bufcombL4,combtuningL4);
	combR[3].setbuffer(bufcombR4,combtuningR4);
	combL[4].setbuffer(bufcombL5,combtuningL5);
	combR[4].setbuffer(bufcombR5,combtuningR5);
	combL[5].setbuffer(bufcombL6,combtuningL6);
	combR[5].setbuffer(bufcombR6,combtuningR6);
	combL[6].setbuffer(bufcombL7,combtuningL7);
	combR[6].setbuffer(bufcombR7,combtuningR7);
	combL[7].setbuffer(bufcombL8,combtuningL8);
	combR[7].setbuffer(bufcombR8,combtuningR8);
	allpassL[0].setbuffer(bufallpassL1,allpasstuningL1);
	allpassR[0].setbuffer(bufallpassR1,allpasstuningR1);
	allpassL[1].setbuffer(bufallpassL2,allpasstuningL2);
	allpassR[1].setbuffer(bufallpassR2,allpasstuningR2);
	allpassL[2].setbuffer(bufallpassL3,allpasstuningL3);
	allpassR[2].setbuffer(bufallpassR3,allpasstuningR3);
	allpassL[3].setbuffer(bufallpassL4,allpasstuningL4);
	allpassR[3].setbuffer(bufallpassR4,allpasstuningR4);

	// Set default values
	allpassL[0].setfeedback(0.5f);
	allpassR[0].setfeedback(0.5f);
	allpassL[1].setfeedback(0.5f);
	allpassR[1].setfeedback(0.5f);
	allpassL[2].setfeedback(0.5f);
	allpassR[2].setfeedback(0.5f);
	allpassL[3].setfeedback(0.5f);
	allpassR[3].setfeedback(0.5f);
	setwet(initialwet);
	setroomsize(initialroom);
	setdry(initialdry);
	setdamp(initialdamp);
	setwidth(initialwidth);
	setmode(initialmode);

	// Buffer will be full of rubbish - so we MUST mute them
	mute();
}

// PROVEN: fecbe000 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD74C0/0x1408A2720
void revmodel::mute()
{
	int i;

	if (getmode() >= freezemode)
		return;

	for (i=0;i<numcombs;i++)
	{
		combL[i].mute();
		combR[i].mute();
	}
	for (i=0;i<numallpasses;i++)
	{
		allpassL[i].mute();
		allpassR[i].mute();
	}
}

// PROVEN: bce099b3 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD6E00/0x1408A2010
void revmodel::processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;
	int i;

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * gain;

		// Accumulate comb filters in parallel
		for(i=0; i<numcombs; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}

		// Calculate output REPLACING anything already there		
		*outputL = outL*wet1 + outR*wet2 + *inputL*dry;
		*outputR = outR*wet1 + outL*wet2 + *inputR*dry;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

void revmodel::processmix(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;
	int i;

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * gain;

		// Accumulate comb filters in parallel
		for(i=0; i<numcombs; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}

		// Calculate output MIXING with anything already there
		*outputL += outL*wet1 + outR*wet2 + *inputL*dry;
		*outputR += outR*wet1 + outL*wet2 + *inputR*dry;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

// PROVEN: e28e1735 IDENTICAL/UNPROVEN
// OFFSET: 0x00AD7290/0x1408A2500
void revmodel::update()
{
// Recalculate internal values after parameter change

	int i;

	wet1 = wet*(width/2 + 0.5f);
	wet2 = wet*((1-width)/2);

	if (mode >= freezemode)
	{
		roomsize1 = 1;
		damp1 = 0;
		gain = muted;
	}
	else
	{
		roomsize1 = roomsize;
		damp1 = damp;
		gain = fixedgain;
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setfeedback(roomsize1);
		combR[i].setfeedback(roomsize1);
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setdamp(damp1);
		combR[i].setdamp(damp1);
	}
}

// The following get/set functions are not inlined, because
// speed is never an issue when calling them, and also
// because as you develop the reverb model, you may
// wish to take dynamic action when they are called.

// PROVEN: 0f3792f2 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD7380/0x1408A2610
void revmodel::setroomsize(float value)
{
	roomsize = (value*scaleroom) + offsetroom;
	update();
}

// PROVEN: f52b1539 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD73B0/0x1408A2630
float revmodel::getroomsize()
{
	return (roomsize-offsetroom)/scaleroom;
}

// PROVEN: 01a8adb3 IDENTICAL/NONE
// OFFSET: 0x00AD73C0
void revmodel::setdamp(float value)
{
	damp = value*scaledamp;
	update();
}

// PROVEN: 078adc5e IDENTICAL/IDENTICAL
// OFFSET: 0x00AD73E0/0x1408A2670
float revmodel::getdamp()
{
	return damp/scaledamp;
}

// PROVEN: e3cdeece IDENTICAL/NONE
// OFFSET: 0x00AD73F0
void revmodel::setwet(float value)
{
	wet = value*scalewet;
	update();
}

// PROVEN: 53c69a51 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD7410/0x1408A2670
float revmodel::getwet()
{
	return wet/scalewet;
}

// PROVEN: 662859fd IDENTICAL/NONE
// OFFSET: 0x00AD7420
void revmodel::setdry(float value)
{
	dry = value*scaledry;
}

// PROVEN: bc7ec611 IDENTICAL/DIFFERENT
// OFFSET: 0x00AD7440/0x1408A26A0
float revmodel::getdry()
{
	return dry/scaledry;
}

// PROVEN: 6009eb56 IDENTICAL/NONE
// OFFSET: 0x00AD7450
void revmodel::setwidth(float value)
{
	width = value;
	update();
}

// PROVEN: 4f5ecfcb IDENTICAL/IDENTICAL
// OFFSET: 0x00AD7470/0x1408A26E0
float revmodel::getwidth()
{
	return width;
}

// PROVEN: 70d1fc13 IDENTICAL/NONE
// OFFSET: 0x00AD7480
void revmodel::setmode(float value)
{
	mode = value;
	update();
}

// PROVEN: a49e4cd6 IDENTICAL/IDENTICAL
// OFFSET: 0x00AD74A0/0x1408A2700
float revmodel::getmode()
{
	if (mode >= freezemode)
		return 1;
	else
		return 0;
}

//ends
