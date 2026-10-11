#include <stdarg.h>
#include <stdio.h>

#include "fmod_string.h"
#include "fmod_memory.h"

using namespace FMOD;
     
// PROVEN: 3a03a450 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC32D0/0x14088BBE0
int FMOD_strlen(const char * s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc) /* nothing */;
    
    return (int)(sc - s);
}

// PROVEN: 20a99f43 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC32F0/0x14088BC00
char * FMOD_strcpy(char * dest,const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0') /* nothing */;
    
    return tmp;
}

// PROVEN: 6b155366 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC3310/0x14088BC30
char * FMOD_strncpy(char * dest, const char *src, int count)
{
    char *tmp = dest;

    while (count-- && (*dest++ = *src++) != '\0') /* nothing */;

    return tmp;
}


// PROVEN: 3f5200b2 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC3340/0x14088BC60
char * FMOD_strcat(char * dest, const char * src)
{
    char *tmp = dest;

    while (*dest)   dest++;
    while ((*dest++ = *src++) != '\0') ;

    return tmp;
}

// PROVEN: d5e1d4ed IDENTICAL/IDENTICAL
// OFFSET: 0x00AC3370/0x14088BCA0
char * FMOD_strncat(char *dest, const char *src, int count)
{
    char *tmp = dest;

    if (count) 
    {
        while (*dest) dest++;
        
        while ((*dest++ = *src++)) 
        {
            if (--count == 0) 
            {
                *dest = '\0';
                break;
            }
        }
    }

    return tmp;
}

char FMOD_tolower(char in)
{
    if (in >= 'A' && in <= 'Z')
    {
        in += 'a' - 'A';
    }

    return in;
}

// PROVEN: ef5ce90d IDENTICAL/IDENTICAL
// OFFSET: 0x00AC33C0/0x14088BCF0
char *FMOD_strupr(char *string)
{
    char * cp;

    for (cp=string; *cp; ++cp)
    {
        if (*cp >= 'a' && *cp <= 'z')
        {
            *cp += 'A' - 'a';
        }
    }

    return(string);
}

char *FMOD_strlwr(char *string)
{
    char * cp;

    for (cp=string; *cp; ++cp)
    {
        if (*cp >= 'A' && *cp <= 'Z')
        {
            *cp += 'a' - 'A';
        }
    }

    return(string);
}

// INLINED
int FMOD_strcmp( const char *string1, const char *string2 )
{
    register char c1, c2;
    do 
    {
        c1 = *string1++;
        c2 = *string2++;
    } while ( c1 && (c1 == c2) );

    return c1 - c2;
}


// INLINED
int FMOD_strncmp( const char *string1, const char *string2, int len )
{
    register char c1, c2;
    int count=0;
    do 
    {
        c1 = *string1++;
        c2 = *string2++;
        count++;
    } while ( c1 && (c1 == c2) && count < len);

    return c1 - c2;
}


// INLINED
int FMOD_stricmp( const char *string1, const char *string2 )
{
    register char c1, c2;
    do 
    {
        c1 = FMOD_tolower(*string1++);
        c2 = FMOD_tolower(*string2++);
    } while ( c1 && (c1 == c2) );

    return c1 - c2;
}

// INLINED
int FMOD_strnicmp( const char *string1, const char *string2, int len )
{
    register char c1, c2;
    int count=0;
    do 
    {
        c1 = FMOD_tolower(*string1++);
        c2 = FMOD_tolower(*string2++);
        count++;
    } while ( c1 && (c1 == c2) && count < len);

    return c1 - c2;
}

char * FMOD_strchr(const char * s1, int c)
{
    int l1;
    
    l1 = FMOD_strlen(s1);
    
    while (l1) 
    {
        l1--;

        if (*s1 == c) return (char *) s1;

        s1++;
    }
    return (char *)0;
}

// PROVEN: 4e0e61df IDENTICAL/IDENTICAL
// OFFSET: 0x00AC3610/0x14088BF60
char * FMOD_strstr(const char * s1,const char * s2)
{
    int l1, l2;

    l2 = FMOD_strlen(s2);

    if (!l2)
    {
        return (char *) s1;
    }
    
    l1 = FMOD_strlen(s1);
    
    while (l1 >= l2) 
    {
        l1--;

        if (!FMOD_memcmp(s1,s2,l2)) return (char *) s1;

        s1++;
    }
    return (char *)0;
}


int FMOD_memcmp(const void * cs,const void * ct, int count)
{
    const unsigned char *su1, *su2;
    signed char res = 0;

    for( su1 = (const unsigned char *)cs, su2 = (const unsigned char *)ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;

    return res;
}


// PROVEN: 781c5c27 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC3530/0x14088BE50
void * FMOD_memmove(void * dest,const void *src, int count)
{
    char *tmp, *s;

    if (dest <= src) 
    {
        tmp = (char *) dest;
        s = (char *) src;
        while (count--) *tmp++ = *s++;
    }
    else 
    {
        tmp = (char *) dest + count;
        s = (char *) src + count;
        while (count--) *--tmp = *--s;
    }

    return dest;
}


// PROVEN: 0f32b136 IDENTICAL/DIFFERENT
// OFFSET: 0x00AC3590/0x14088BEC0
char *FMOD_strdup(const char * src)
{
    char *ret = 0;

    ret = (char *)FMOD_Memory_Alloc(FMOD_strlen(src) + 1);
    if (ret)
    {
        FMOD_strcpy(ret, src);
    }

    return ret;
}


// PROVEN: 0c6270d4 IDENTICAL/IDENTICAL
// OFFSET: 0x00AC35E0/0x14088BF40
char *FMOD_eatwhite(const char *string)
{
    for (;*string && FMOD_isspace(*string);string++) ;

    return (char *)string;
}


/*
    AJS This only handles decimal integers. Feel free to extend it if you need to
*/
int FMOD_atoi(const char *s)
{
    if (!s)
    {
        return 0;
    }

    int   val     = 0;
    const char *p = s;

    for (; *p; p++) ;

    p--;
    for (int i=1; p >= s; p--, i *= 10)
    {
        val += ((*p - '0') * i);
    }

    return val;
}

int FMOD_atoi_hex(const char *s)
{
    if (!s)
    {
        return 0;
    }

    int val = 0;

    for(const char *p = s; *p; p++)
    {
        val *= 16;

        if(*p >= '0' && *p <= '9')
        {
            val += (*p - '0');
        }
        else if(*p >= 'A' && *p <= 'F')
        {
            val += (*p - 'A' + 10);
        }
        else if(*p >= 'a' && *p <= 'f')
        {
            val += (*p - 'a' + 10);
        }
    }

    return val;
}

int FMOD_snprintf(char *str, int size, const char *format, ...)
{
    va_list arglist;

    va_start(arglist, format);

    int result = FMOD_vsnprintf(str, size, format, arglist);

    va_end(arglist);

    return result;
}

int FMOD_vsnprintf(char *str, int size, const char *format, va_list arglist)
{
#ifdef PLATFORM_PS2_IOP
    int result = vsprintf(str, format, arglist);
#else
    int result = vsnprintf(str, size, format, arglist);
#endif

    str[size - 1] = 0;

    return result;
}
