#ifndef _FMOD_LOCALCRITICALSECTION_H
#define _FMOD_LOCALCRITICALSECTION_H

#include "fmod_settings.h"
#include "fmod_os_misc.h"

namespace FMOD
{
    class LocalCriticalSection
    {
      private:

        FMOD_OS_CRITICALSECTION    *mCrit;        
        bool                        mEntered;

      public:

        LocalCriticalSection() 
        {
            mEntered = false;
        }
        LocalCriticalSection(FMOD_OS_CRITICALSECTION *crit, bool enternow = false)
        {
            mEntered = false;
            mCrit = crit;

            if (enternow)
            {
                enter();
            }
        }
        // INLINED
        ~LocalCriticalSection() 
        { 
            if (mEntered)
            {
                leave();
            }
        }

        // INLINED
        void enter()
        {
            FMOD_OS_CriticalSection_Enter(mCrit);
            mEntered = true;
        }
        // INLINED
        void leave()
        {
            FMOD_OS_CriticalSection_Leave(mCrit);
            mEntered = false;
        }
   };
}

#endif


