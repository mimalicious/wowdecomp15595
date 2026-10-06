#include "fmod_settings.h"

#include "fmod_channeli.h"
#include "fmod_channel_real.h"
#include "fmod_channelpool.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_channel_software.h"

#include "fmod_systemi.h"

namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: e14280a8 IDENTICAL/NONE
// OFFSET: 0x00A934A0/0x140853250
ChannelPool::ChannelPool()
{
    mChannel      = 0;
    mNumChannels  = 0;
    mChannelsUsed = 0;
    mSystem       = 0;
    mOutput       = 0;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: d7788da8 IDENTICAL/NONE
// OFFSET: 0x00A934C0/0x140853270
FMOD_RESULT ChannelPool::init(SystemI *system, Output *output, int numchannels)
{
    if (numchannels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numchannels)
    {
        mChannel = (ChannelReal **)FMOD_Memory_Calloc(numchannels * sizeof(ChannelReal *));
        if (!mChannel)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }
    
    mNumChannels = numchannels;
    mSystem = system;
    mOutput = output;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 7d9eefe8 IDENTICAL/NONE
// OFFSET: 0x00A93520/0x140853300
FMOD_RESULT ChannelPool::release()
{
    int count;

    if (mChannel)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            if (mChannel[count])
            {
                mChannel[count]->close();
            }
        }

        FMOD_Memory_Free(mChannel);
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 56e634e4 IDENTICAL/NONE
// OFFSET: 0x00A93590/0x1408533B0
FMOD_RESULT ChannelPool::allocateChannel(ChannelReal **realchannel, int index, int numchannels, int *found_out)
{
    int count;
    int found = 0;

    if (!realchannel)
    {
        if (found_out)
        {
            *found_out = found;
        }
        return FMOD_ERR_INVALID_PARAM;
    } 

    if (index == FMOD_CHANNEL_FREE)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            FMOD_RESULT result;
            bool playing;
        
            if (mChannel[count]->mFlags & CHANNELREAL_FLAG_ALLOCATED || 
                mChannel[count]->mFlags & CHANNELREAL_FLAG_IN_USE ||
                mChannel[count]->mFlags & CHANNELREAL_FLAG_ENDDELAY ||
                mChannel[count]->mFlags & CHANNELREAL_FLAG_RESERVED)
            {
                continue;
            }
        
            result = mChannel[count]->isPlaying(&playing, true);
            if (result == FMOD_OK && !playing)
            {
                mChannel[count]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
                mChannel[count]->mFlags |= CHANNELREAL_FLAG_IN_USE;
                mChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;

                realchannel[found] = mChannel[count];
                found++;
                if (found == numchannels)
                {
                    if (found_out)
                    {
                        *found_out = found;
                    }
                    return FMOD_OK;
                }
            }
        }
    }
    else if (index >= 0 && index < mNumChannels)
    {  
        if (numchannels > 1)
        {
            return FMOD_ERR_CHANNEL_ALLOC;
        }

        mChannel[index]->mFlags |= CHANNELREAL_FLAG_ALLOCATED;
        mChannel[index]->mFlags |= CHANNELREAL_FLAG_IN_USE;
        mChannel[index]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;

        realchannel[0] = mChannel[index];
        return FMOD_OK;
    }

    /*
        Undo the allocations that occured before just in case they are not used.
    */
    for (count = 0; count < found; count++)
    {
        if (realchannel[count])
        {
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
            realchannel[count]->mFlags &= ~CHANNELREAL_FLAG_IN_USE;
            realchannel[count]->mFlags |= CHANNELREAL_FLAG_STOPPED;
        }
    }

    if (found_out)
    {
        *found_out = found;
    }

    return FMOD_ERR_CHANNEL_ALLOC;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 5a97e310 IDENTICAL/NONE
// OFFSET: 0x00A936F0/0x1408535B0
FMOD_RESULT ChannelPool::getNumChannels(int *numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mNumChannels;

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 643955d9 IDENTICAL/NONE
// OFFSET: 0x00A93710
FMOD_RESULT ChannelPool::getChannelsUsed(int *numchannels)
{
    if (!numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numchannels = mChannelsUsed;
    
    return FMOD_OK;
}

 
/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 0a6ea6d7 IDENTICAL/NONE
// OFFSET: 0x00A93730/0x1408BCA80
FMOD_RESULT ChannelPool::setChannel(int index, ChannelReal *channel, DSPI *dspmixtarget)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mChannel[index] = channel;
    mChannel[index]->mPool = this;

    return mChannel[index]->init(index, mSystem, mOutput, dspmixtarget);
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]

	[SEE_ALSO]
]
*/
// PROVEN: 0df481f3 IDENTICAL/NONE
// OFFSET: 0x00A93780
FMOD_RESULT ChannelPool::getChannel(int index, ChannelReal **channel)
{
    if (!channel || index < 0 || index >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channel = mChannel[index];

    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]
    FMOD_OK

	[REMARKS]

    [PLATFORMS]
    
	[SEE_ALSO]
]
*/


}
