// Comb filter implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "comb.h"

// OFFSET: 0x00AF5350/0x1408BE3C0
comb::comb()
{
	filterstore = 0;
	bufidx = 0;
}

// OFFSET: 0x00AF5370/0x1408BE3D0
void comb::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

// OFFSET: 0x00AF5390/0x1408BE3E0
void comb::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}

// OFFSET: 0x00AF53B0/0x1408BE410
void comb::setdamp(float val) 
{
	damp1 = val; 
	damp2 = 1-val;
}

float comb::getdamp() 
{
	return damp1;
}

void comb::setfeedback(float val) 
{
	feedback = val;
}

float comb::getfeedback() 
{
	return feedback;
}

// ends
