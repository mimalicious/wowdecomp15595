// Allpass filter implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "allpass.h"

// PROVEN: fa2b1f93 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF53E0/0x1408BE440
allpass::allpass()
{
	bufidx = 0;
}

// PROVEN: 628ab753 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF53F0/0x1408BE450
void allpass::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

// PROVEN: 298165f3 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF5410/0x1408BE460
void allpass::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}

// PROVEN: 89ec25fd IDENTICAL/IDENTICAL
// OFFSET: 0x00AF5430/0x1408BE430
void allpass::setfeedback(float val) 
{
	feedback = val;
}

float allpass::getfeedback() 
{
	return feedback;
}

//ends
