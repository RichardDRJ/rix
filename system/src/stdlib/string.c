#include "string.h"
#include "palloc.h"
#include "system.h"

char strcmp(char *str1, char *str2)
{
    while(*str1 || *str2)
    {
        if(*str1 < *str2)
            return -1;
        else if(*str1 > *str2)
            return 1;
        ++str1;
        ++str2;
    }

    return 0;
}

char iswhitespace(char in)
{
    return (in == ' ' || in == '\n' || in == '\t' || in == '\r');
}

char strcontains(char *str, char c)
{
    while(*str != 0)
    {
        if(*str == c)
            return 1;
        ++str;
    }
    return 0;
}

char *strchr(const char *s, int c)
{
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

unsigned int strcspn(const char *s1, const char *s2)
{
    unsigned int ret=0;
    while(*s1)
        if(strchr(s2,*s1))
            return ret;
        else
            ++s1,++ret;
    return ret;
}

unsigned int strspn(const char *s1, const char *s2)
{
    unsigned int ret=0;
    while(*s1 && strchr(s2,*s1++))
        ++ret;
    return ret;    
}

char *strtok(char * str, const char * delim)
{
    static char* p=0;
    if(str)
        p=str;
    else if(!p)
        return 0;
    str=p+strspn(p,delim);
    p=str+strcspn(str,delim);
    if(p==str)
        return p=0;
    p = *p ? *p=0,p+1 : 0;
    return str;
}

/*char *strtok(char *str, char *delimiters)
  {
  static char *strtail;
  if(str != 0)
  strtail = str;

  if(*strtail == 0)
  return 0;

  while(strcontains(delimiters, *strtail))
  {
  ++strtail;
  }

  char *dfinder = strtail;

  if(*dfinder == 0)
  return 0;

  while(!strcontains(delimiters, *dfinder) && *dfinder != 0)
  {
  ++dfinder;
  }

  char *retValue = strtail;

  if(*dfinder == 0)
  {
  strtail = dfinder;
  }
  else
  {
 *dfinder = 0;
 strtail = dfinder + 1;
 }

 return retValue;
 }*/

char toUpper(char in)
{
    if(in >= 'a' && in <= 'z')
        in += ('A' - 'a');
    return in;
}

char *filenametoshort(char *fname)
{
    char *rets = palloc(12 * sizeof(char));
    unsigned short i = 0;
    for(; i < 8; ++i)
    {
        if(fname[i] == '.')
            break;
        rets[i] = toUpper(fname[i]);
    }
    for(; i < 8; ++i)
        rets[i] = ' ';

    while(*fname != '.')
        ++fname;

    ++fname;

    for(i = 8; i < 11; ++i)
    {
        if(*fname == 0)
            break;
        rets[i] = toUpper(*fname++);
    }
    for(; i < 11; ++i)
        rets[i] = ' ';

    rets[11] = 0;

    return rets;
}
