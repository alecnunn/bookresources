/******************************************************************************

Coral Tree gpl/tools.cc
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#include <gpl/gpl.h>

char *gplStrDup(const char *source)
{
  char * newString = new char[strlen(source)+1];

  strcpy(newString,source);
  return newString;
}

char *gplStrDup(const char *source, unsigned long length)
{
  char * newString = new char[length];

  if(length > strlen(source)+1) length = strlen(source)+1;
  strncpy(newString,source,(unsigned int)length);
  return newString;
}

char *gplExtract(const char *string, char *target, const char *sep, int pos)
{
  char *workingCopy;
  char *start;
  char *end;
  size_t genLen;

  workingCopy=gplStrDup(string);
  start=workingCopy;

  genLen=strlen(sep);
  if(target) target[0]='\0';
  gplStripLeadingSpaces(&start);     //skip leading white spaces

// Iterate through the list until pos == 0 or the string is exhausted.

  end=strstr(start,sep);
  while ((pos) && (end)) {
         start= end+genLen;
         end= start ? strstr(start,sep): (char *)NULL;
         pos--;
  }

// if the string is exhausted or too tired to continue, return a NULL.

  if ( ((NULL==start) || ('\0' == *start)) || (0 != pos) )  {
     delete[] workingCopy;
	 return (char *)NULL;
  }

// Eat all the spaces surrounding the target token. copy the string.

  gplStripLeadingSpaces(&start);

  genLen=end ? (end - start): strlen(start);
  if(target) 
  {
    strncpy(target,start,genLen);
  }
  else 
  {
    target = gplStrDup(start, genLen + 1);
  }
  target[genLen]='\0';

  gplStripTrailingSpaces(target,genLen);

 delete[] workingCopy;
 return target;
}


// Will modify the pointer passed to it.
void  gplStripLeadingSpaces(char **string)
{
  if (0 == *string)
	return;

  while(isspace(**string) && (**string))
     (*string)++;
}

// Will modify the pointer passed to it.
void  gplStripTrailingSpaces(char *target,int genLen)
{
  register int i=genLen-1;
  while (isspace(target[i]) && ( 0 <=i )) {
    target[i]='\0';
	i--;
  }
}

