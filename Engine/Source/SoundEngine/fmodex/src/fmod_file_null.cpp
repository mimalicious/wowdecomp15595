#include "fmod_settings.h"

#include "fmod_file_null.h"

#include <stdio.h>

namespace FMOD
{

/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// PROVEN: 9e24e2aa IDENTICAL
// OFFSET: 0x00A937B0
FMOD_RESULT NullFile::reallyOpen(const char *name_or_data, unsigned int *filesize)
{
	mPosition = 0;

    *filesize = 0;  /* This will be set by the user after this call? */

	return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
FMOD_RESULT NullFile::reallyClose()
{
    return FMOD_OK;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// PROVEN: df80dd1e IDENTICAL
// OFFSET: 0x00A937D0
FMOD_RESULT NullFile::reallyRead(void *buffer, unsigned int size, unsigned int *read)
{
    FMOD_RESULT result = FMOD_OK;

	if (mPosition + size > mLength)
    {
		size = mLength - mPosition;

        result = FMOD_ERR_INVALID_PARAM;
    }

    *read = size;

    mPosition += size;

    return result;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/
// PROVEN: 71d54def IDENTICAL
// OFFSET: 0x00A92BA0
FMOD_RESULT NullFile::reallySeek(unsigned int pos)
{
    mPosition = pos;

    if (mPosition > mLength)
    {
        mPosition = mLength;
    }
    if (mPosition < 0)
    {
        mPosition = 0;
    }

    return FMOD_OK;
}



/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

	[SEE_ALSO]
]
*/

}

