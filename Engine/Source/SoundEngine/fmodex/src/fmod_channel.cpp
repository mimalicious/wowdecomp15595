/*$ preserve start $*/

#include "fmod_settings.h"

#include "fmod.hpp"
#include "fmod_channeli.h"
#include "fmod_systemi.h"

namespace FMOD
{
/*$ preserve end $*/


FMOD_RESULT Channel::getSystemObject(System **system)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (system)
        {
            *system = 0;
        }
        return result;
    }
    else
    {
        return channeli->getSystemObject(system);
    }
}


// PROVEN: 56601f87 IDENTICAL/IDENTICAL
// OFFSET: 0x00A7A8D0/0x1408359E0
FMOD_RESULT Channel::stop()
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->stop();
    }
}


// OFFSET: 0x00A7A900/0x140835A10
FMOD_RESULT Channel::setPaused(bool paused)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPaused(paused);
    }
}


FMOD_RESULT Channel::getPaused(bool *paused)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (paused)
        {
            *paused = false;
        }
        return result;
    }
    else
    {
        return channeli->getPaused(paused);
    }
}


// PROVEN: a8ac643d IDENTICAL/IDENTICAL
// OFFSET: 0x00A7A930/0x140835A40
FMOD_RESULT Channel::setVolume(float volume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setVolume(volume);
    }
}


// INLINED
FMOD_RESULT Channel::getVolume(float *volume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (volume)
        {
            *volume = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getVolume(volume);
    }
}


// OFFSET: 0x00A7A960/0x140835A80
FMOD_RESULT Channel::setFrequency(float frequency)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setFrequency(frequency);
    }
}


// OFFSET: 0x00A7A990
FMOD_RESULT Channel::getFrequency(float *frequency)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (frequency)
        {
            *frequency = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getFrequency(frequency);
    }
}


FMOD_RESULT Channel::setPan(float pan)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPan(pan);
    }
}


FMOD_RESULT Channel::getPan(float *pan)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (pan)
        {
            *pan = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getPan(pan);
    }
}


FMOD_RESULT Channel::setDelay(unsigned int startdelay, unsigned int enddelay)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setDelay(startdelay, enddelay);
    }
}


FMOD_RESULT Channel::getDelay(unsigned int *startdelay, unsigned int *enddelay)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (startdelay)
        {
            *startdelay = 0;
        }
        if (enddelay)
        {
            *enddelay = 0;
        }
        return result;
    }
    else
    {
        return channeli->getDelay(startdelay, enddelay);
    }
}


// PROVEN: 4a177c92 IDENTICAL/IDENTICAL
// OFFSET: 0x00A7A9D0/0x140835B00
FMOD_RESULT Channel::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    }
}


FMOD_RESULT Channel::getSpeakerMix(float *frontleft, float *frontright, float *center, float *lfe, float *backleft, float *backright, float *sideleft, float *sideright)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (frontleft)
        {
            *frontleft = 0.0f;
        }
        if (frontright)
        {
            *frontright = 0.0f;
        }
        if (center)
        {
            *center = 0.0f;
        }
        if (lfe)
        {
            *lfe = 0.0f;
        }
        if (backleft)
        {
            *backleft = 0.0f;
        }
        if (backright)
        {
            *backright = 0.0f;
        }
        if (sideleft)
        {
            *sideleft = 0.0f;
        }
        if (sideright)
        {
            *sideright = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    }
}


FMOD_RESULT Channel::setSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setSpeakerLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT Channel::getSpeakerLevels(FMOD_SPEAKER speaker, float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (levels)
        {
            *levels = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpeakerLevels(speaker, levels, numlevels);
    }
}


FMOD_RESULT Channel::setInputChannelMix(float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setInputChannelMix(levels, numlevels);
    }
}


FMOD_RESULT Channel::getInputChannelMix(float *levels, int numlevels)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (levels)
        {
            *levels = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getInputChannelMix(levels, numlevels);
    }
}


FMOD_RESULT Channel::setMute(bool mute)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setMute(mute);
    }
}


FMOD_RESULT Channel::getMute(bool *mute)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mute)
        {
            *mute = false;
        }
        return result;
    }
    else
    {
        return channeli->getMute(mute);
    }
}


// OFFSET: 0x00A7AA30
FMOD_RESULT Channel::setPriority(int priority)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPriority(priority);
    }
}


// INLINED
FMOD_RESULT Channel::getPriority(int *priority)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (priority)
        {
            *priority = 0;
        }
        return result;
    }
    else
    {
        return channeli->getPriority(priority);
    }
}


// OFFSET: 0x00A7AA60/0x140835BE0
FMOD_RESULT Channel::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setPosition(position, postype);
    }
}


// PROVEN: c7cb4153 IDENTICAL/IDENTICAL
// OFFSET: 0x00A7AA90/0x140835C20
FMOD_RESULT Channel::getPosition(unsigned int *position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (position)
        {
            *position = 0;
        }
        return result;
    }
    else
    {
        return channeli->getPosition(position, postype);
    }
}


// OFFSET: 0x00A7AAD0
FMOD_RESULT Channel::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setReverbProperties(prop);
    }
}


// OFFSET: 0x00A7AB00
FMOD_RESULT Channel::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->getReverbProperties(prop);
    }
}
// PROVEN: 484cfc06 IDENTICAL/IDENTICAL
// OFFSET: 0x00A7AB30/0x140835CD0
FMOD_RESULT Channel::setChannelGroup(ChannelGroup *channelgroup)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setChannelGroup((ChannelGroupI *)channelgroup);
    }
}


FMOD_RESULT Channel::getChannelGroup(ChannelGroup **channelgroup)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (channelgroup)
        {
            *channelgroup = 0;
        }
        return result;
    }
    else
    {
        return channeli->getChannelGroup((ChannelGroupI **)channelgroup);
    }
}


// OFFSET: 0x00A7AB60/0x140835D00
FMOD_RESULT Channel::setCallback(FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setCallback(type, callback, command);
    }
}


// OFFSET: 0x00A7AB90
FMOD_RESULT Channel::set3DAttributes(const FMOD_VECTOR *pos, const FMOD_VECTOR *vel)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DAttributes(pos, vel);
    }
}


// INLINED
FMOD_RESULT Channel::get3DAttributes(FMOD_VECTOR *pos, FMOD_VECTOR *vel)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (pos)
        {
            pos->x = pos->y = pos->z = 0.0f;
        }
        if (vel)
        {
            vel->x = vel->y = vel->z = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DAttributes(pos, vel);
    }
}


// PROVEN: 485e6d60 IDENTICAL/IDENTICAL
// OFFSET: 0x00A7ABC0/0x140835D90
FMOD_RESULT Channel::set3DMinMaxDistance(float mindistance, float maxdistance)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DMinMaxDistance(mindistance, maxdistance);
    }
}


// OFFSET: 0x00A7AC00
FMOD_RESULT Channel::get3DMinMaxDistance(float *mindistance, float *maxdistance)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mindistance)
        {
            *mindistance = 0.0f;
        }
        if (maxdistance)
        {
            *maxdistance = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DMinMaxDistance(mindistance, maxdistance);
    }
}


// OFFSET: 0x00A7AC50
FMOD_RESULT Channel::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    }
}


FMOD_RESULT Channel::get3DConeSettings(float *insideconeangle, float *outsideconeangle, float *outsidevolume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (insideconeangle)
        {
            *insideconeangle = 0.0f;
        }
        if (outsideconeangle)
        {
            *outsideconeangle = 0.0f;
        }
        if (outsidevolume)
        {
            *outsidevolume = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    }
}


// OFFSET: 0x00A7AC90
FMOD_RESULT Channel::set3DConeOrientation(FMOD_VECTOR *orientation)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DConeOrientation(orientation);
    }
}


FMOD_RESULT Channel::get3DConeOrientation(FMOD_VECTOR *orientation)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (orientation)
        {
            orientation->x = orientation->y = orientation->z = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DConeOrientation(orientation);
    }
}


FMOD_RESULT Channel::set3DCustomRolloff(FMOD_VECTOR *points, int numpoints)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DCustomRolloff(points, numpoints);
    }
}


FMOD_RESULT Channel::get3DCustomRolloff(FMOD_VECTOR **points, int *numpoints)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (points)
        {
            *points = 0;
        }
        if (numpoints)
        {
            *numpoints = 0;
        }
        return result;
    }
    else
    {
        return channeli->get3DCustomRolloff(points, numpoints);
    }
}


FMOD_RESULT Channel::set3DOcclusion(float directocclusion, float reverbocclusion)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DOcclusion(directocclusion, reverbocclusion);
    }
}


FMOD_RESULT Channel::get3DOcclusion(float *directocclusion, float *reverbocclusion)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (directocclusion)
        {
            *directocclusion = 0.0f;
        }
        if (reverbocclusion)
        {
            *reverbocclusion = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DOcclusion(directocclusion, reverbocclusion);
    }
}


FMOD_RESULT Channel::set3DSpread(float angle)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DSpread(angle);
    }
}


FMOD_RESULT Channel::get3DSpread(float *angle)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (angle)
        {
            *angle = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DSpread(angle);
    }
}


// OFFSET: 0x00A7ACC0
FMOD_RESULT Channel::set3DPanLevel(float level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DPanLevel(level);
    }
}


// INLINED
FMOD_RESULT Channel::get3DPanLevel(float *level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (level)
        {
            *level = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DPanLevel(level);
    }
}


FMOD_RESULT Channel::set3DDopplerLevel(float level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->set3DDopplerLevel(level);
    }
}


FMOD_RESULT Channel::get3DDopplerLevel(float *level)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (level)
        {
            *level = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->get3DDopplerLevel(level);
    }
}


FMOD_RESULT Channel::getDSPHead(DSP **dsp)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (dsp)
        {
            *dsp = 0;
        }
        return result;
    }
    else
    {
        return channeli->getDSPHead((DSPI **)dsp);
    }
}


// PROVEN: 16bcdaee IDENTICAL/IDENTICAL
// OFFSET: 0x00A7ACF0/0x140835F10
FMOD_RESULT Channel::addDSP(DSP *dsp)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->addDSP((DSPI *)dsp);
    }
}


// OFFSET: 0x00A7AD20
FMOD_RESULT Channel::isPlaying(bool *isplaying)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (isplaying)
        {
            *isplaying = false;
        }
        return result;
    }
    else
    {
        return channeli->isPlaying(isplaying);
    }
}


// OFFSET: 0x00A7AD60/0x140835F80
FMOD_RESULT Channel::isVirtual(bool *isvirtual)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (isvirtual)
        {
            *isvirtual = false;
        }
        return result;
    }
    else
    {
        return channeli->isVirtual(isvirtual);
    }
}


// OFFSET: 0x00A7ADA0/0x140835FC0
FMOD_RESULT Channel::getAudibility(float *audibility)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (audibility)
        {
            *audibility = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getAudibility(audibility);
    }
}


// INLINED
FMOD_RESULT Channel::getFadeVolume(float *fadevolume)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (fadevolume)
        {
            *fadevolume = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getFadeVolume(fadevolume);
    }
}


FMOD_RESULT Channel::getCurrentSound(Sound **sound)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (sound)
        {
            *sound = 0;
        }
        return result;
    }
    else
    {
        return channeli->getCurrentSound((SoundI **)sound);
    }
}


FMOD_RESULT Channel::getSpectrum(float *spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (spectrumarray)
        {
            *spectrumarray = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getSpectrum(spectrumarray, numvalues, channeloffset, windowtype);
    }
}


FMOD_RESULT Channel::getWaveData(float *wavearray, int numvalues, int channeloffset)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (wavearray)
        {
            *wavearray = 0.0f;
        }
        return result;
    }
    else
    {
        return channeli->getWaveData(wavearray, numvalues, channeloffset);
    }
}


FMOD_RESULT Channel::getIndex(int *index)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (index)
        {
            *index = 0;
        }
        return result;
    }
    else
    {
        return channeli->getIndex(index);
    }
}


FMOD_RESULT Channel::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setMode(mode);
    }
}


// OFFSET: 0x00A7ADE0
FMOD_RESULT Channel::getMode(FMOD_MODE *mode)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (mode)
        {
            *mode = (FMOD_MODE)0;
        }
        return result;
    }
    else
    {
        return channeli->getMode(mode);
    }
}


FMOD_RESULT Channel::setLoopCount(int loopcount)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setLoopCount(loopcount);
    }
}


FMOD_RESULT Channel::getLoopCount(int *loopcount)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (loopcount)
        {
            *loopcount = 0;
        }
        return result;
    }
    else
    {
        return channeli->getLoopCount(loopcount);
    }
}


FMOD_RESULT Channel::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    }
}


FMOD_RESULT Channel::getLoopPoints(unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (loopstart)
        {
            *loopstart = 0;
        }
        if (loopend)
        {
            *loopend = 0;
        }
        return result;
    }
    else
    {
        return channeli->getLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    }
}


// OFFSET: 0x00A7AE20/0x140836040
FMOD_RESULT Channel::setUserData(void *_userdata)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        return result;
    }
    else
    {
        return channeli->setUserData(_userdata);
    }
}


// OFFSET: 0x00A7AE50/0x140836070
FMOD_RESULT Channel::getUserData(void **_userdata)
{
    FMOD_RESULT result;
    ChannelI *channeli;

    result = ChannelI::validate(this, &channeli);
    if (result != FMOD_OK)
    {
        if (_userdata)
        {
            *_userdata = 0;
        }
        return result;
    }
    else
    {
        return channeli->getUserData(_userdata);
    }
}



/*$ preserve start $*/
}
/*$ preserve end $*/
