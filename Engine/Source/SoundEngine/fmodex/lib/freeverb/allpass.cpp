// Allpass filter implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "allpass.h"

// OFFSET: %0x00A35EC0
allpass::allpass()
{
	bufidx = 0;
}

// OFFSET: %0x00A35ED0
void allpass::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

// OFFSET: %0x00A35EF0
void allpass::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}

// OFFSET: #0x00945140
void allpass::setfeedback(float val) 
{
	feedback = val;
}

float allpass::getfeedback() 
{
	return feedback;
}

//ends
