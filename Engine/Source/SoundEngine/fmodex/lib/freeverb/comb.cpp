// Comb filter implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "comb.h"

// PROVEN: 6e73cd74 IDENTICAL
// OFFSET: 0x00AF5350
comb::comb()
{
	filterstore = 0;
	bufidx = 0;
}

// PROVEN: faaeedb9 IDENTICAL
// OFFSET: 0x00AF5370
void comb::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

// PROVEN: 8905e1bc IDENTICAL
// OFFSET: 0x00AF5390
void comb::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}

// PROVEN: 83d40f34 IDENTICAL
// OFFSET: 0x00AF53B0
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
