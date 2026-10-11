#include "fmod_settings.h"

#include "fmod_channeli.h"
#include "fmod_channel_emulated.h"
#include "fmod_debug.h"
#include "fmod_dspi.h"
#include "fmod_sound_stream.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"
#include "fmod_os_misc.h"

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
// INLINED
ChannelEmulated::ChannelEmulated()
{
#ifdef FMOD_SUPPORT_SOFTWARE
    mDSPHead = 0;
#endif
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
// PROVEN: 239bcf0a IDENTICAL/DIFFERENT
// OFFSET: 0x00AF4D40/0x1408BE320
FMOD_RESULT ChannelEmulated::init(int index, SystemI *system, Output *output, DSPI *dspmixtarget)
{
    ChannelReal::init(index, system, output, dspmixtarget);

#ifdef FMOD_SUPPORT_SOFTWARE
    if (!(mSystem->mFlags & FMOD_INIT_SOFTWARE_DISABLE))
    {
        FMOD_RESULT result;
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        FMOD_strcpy(descriptionex.name, "EmulatedChannel DSPHead Unit");
        descriptionex.version   = 0x00010100;
        descriptionex.mCategory = FMOD_DSP_CATEGORY_FILTER;
        descriptionex.mFormat   = FMOD_SOUND_FORMAT_PCMFLOAT;

        mDSPHead = &mDSPHeadMemory;

        result = mSystem->createDSP(&descriptionex, &mDSPHead, false);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
#endif

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
// PROVEN: 9edcf9d3 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF4A50/0x1408BDFA0
FMOD_RESULT ChannelEmulated::alloc()
{
    FMOD_RESULT result;

    result = ChannelReal::alloc();
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        result = mDSPHead->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mParent->mChannelGroup->mDSPHead->addInputQueued(mDSPHead, false, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
#endif

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
// INLINED
FMOD_RESULT ChannelEmulated::stop(bool ignoreenddelay, bool resetstate, bool force)
{
    if (mFlags & CHANNELREAL_FLAG_ENDDELAY && !ignoreenddelay)
    {
        return FMOD_OK;
    }

#ifdef FMOD_SUPPORT_STREAMING
    if (mSound)
    {
        if (mSound->isStream())
        {
            Stream *stream = SAFE_CAST(Stream, mSound);

            if (stream->mChannel)
            {
                stream->mChannel->mFinished = true;
            }
        }
    }
#endif

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        mDSPHead->mFlags &= ~FMOD_DSP_FLAG_ACTIVE;
        mDSPHead->disconnectAll(false, true);
        mDSPHead->mFlags &= ~FMOD_DSP_FLAG_VISITED;
        mDSPHead->mFlags |= FMOD_DSP_FLAG_FINISHED;
        *mDSPHead->mWantsToFinish = 0;
    }
#endif

    ChannelReal::stop(ignoreenddelay, true);

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
FMOD_RESULT ChannelEmulated::stop(bool ignoreenddelay, bool resetstate)
{
    return stop(ignoreenddelay, resetstate, false);
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
// PROVEN: 7d4d1629 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF4B40/0x1408BE0E0
FMOD_RESULT ChannelEmulated::close()
{
    FMOD_RESULT result;

    result = ChannelReal::close();
    if (result != FMOD_OK)
    {
        return result;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    if (mDSPHead)
    {
        mDSPHead->release(false);       /* false = dont free this, as it is not alloced. */
        mDSPHead= 0;
    }
#endif

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
// INLINED
FMOD_RESULT ChannelEmulated::update(int delta)
{
    FMOD_RESULT result;

    result = ChannelReal::update(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!(mFlags & CHANNELREAL_FLAG_PAUSED) && !(mFlags & CHANNELREAL_FLAG_ENDDELAY) && mFlags & CHANNELREAL_FLAG_PLAYING)
    {
        int frequency = (int)(mParent->mPitch3D * mParent->mFrequency);
        int step;

        step = frequency * delta / 1000;
        if (mDirection == CHANNELREAL_PLAYDIR_BACKWARDS)
        {
            step = -step;
        }

        mPosition += step;

        if (mSound)
        {
            if (mMode & FMOD_LOOP_NORMAL || mMode & FMOD_LOOP_BIDI && mLoopCount)
            {
                if (mPosition >= mSound->mLoopStart + mSound->mLoopLength)
                {
                    while (mLoopCount)
                    {
                        if (mMode & FMOD_LOOP_NORMAL)
                        {
                            mPosition -= mSound->mLoopLength;
                        }
                        else if (mMode & FMOD_LOOP_BIDI)
                        {
                            mPosition -= step;
                            mDirection = (mDirection == CHANNELREAL_PLAYDIR_FORWARDS) ? CHANNELREAL_PLAYDIR_BACKWARDS : CHANNELREAL_PLAYDIR_FORWARDS;
                        }

                        if (mLoopCount >= 0)
                        {
                            mLoopCount--;
                        }

                        if (mPosition < mSound->mLoopStart + mSound->mLoopLength)
                        {
                            return FMOD_OK;
                        }
                    }

                    mPosition = mSound->mLength;
                    mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PLAYING);
                }
            }
            else if (mPosition >= mSound->mLength)
            {
                mFlags = (CHANNELREAL_FLAG)(mFlags & ~CHANNELREAL_FLAG_PLAYING);
                mPosition = mSound->mLength;
            }
        }
    }

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
// PROVEN: 4da83d69 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF4CA0/0x1408BE290
FMOD_RESULT ChannelEmulated::isVirtual(bool *isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isvirtual = true;

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
// PROVEN: 6db26084 IDENTICAL/IDENTICAL
// OFFSET: 0x00AF4CC0/0x1408BE2B0
FMOD_RESULT ChannelEmulated::getDSPHead(DSPI **dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

#ifdef FMOD_SUPPORT_SOFTWARE
    *dsp = mDSPHead;
#endif

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
FMOD_RESULT ChannelEmulated::setSpeakerLevels(int speaker, float *levels, int numlevels)
{
    return FMOD_OK;
}


}

