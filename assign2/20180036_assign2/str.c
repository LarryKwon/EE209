/* 권혁태 20180036 assignment 2 , str.c */

#include <assert.h> /* to use assert() */
#include <stdio.h>
#include "str.h"

/* Part 1 */

/* StrGetLength */
/* 
to Get Length of string in the array 
parameter: the pointer to array
return : the size of array 
*/

/*------------------------------------------------------------------------*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*StrCopy*/
/* 
to copy the string in the array
parameter: the pointer to array
pcSrc -> pointer of array copy (origin)
pcDest -> pointer of array copy (destination)
return: the start of destinationi array
*/

/*-----------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  assert(pcDest);
  assert(pcSrc);

  char *destination;
  int count =0;
  destination = pcDest;
  const char* search = pcSrc;
  while(*search!='\0'){
    *pcDest = *search;
    search++;
    count ++;
    pcDest++;
  }
  *pcDest='\0';
  return destination;
}

/*StrCompare*/
/* 
to compare the string in the arrays
parameter: the pointer to array
pcS1 -> string 1 for comparing
pcS2 -> string 2 for comparing
return:
if they are same, return 0;
if they are not same, return the difference of ASCII values of 
the characters that are different.
*/

/*-----------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  assert(pcS1);
  assert(pcS2);
  int res=0;
  size_t strlenS1 = StrGetLength(pcS1);
  size_t strlenS2 = StrGetLength(pcS2);
  int count = 0;
  while(count <= strlenS1 && count <= strlenS2){
    if(*pcS1 != *pcS2){
      res = *pcS1 - *pcS2;
      return res;
    }
    pcS1++;
    pcS2++;
    count++;
  }
  return res;
}


/*StrSearch*/
/* 
to check whether the array has specified string

parameter: the pointers to array
pcNeedle -> the string that client want to find
pcHaystack -> the array that client want to check
whether it contains pcNeedle

return:
if pcHaystack contains, return the pointer to address of 
starting point of the detected pcNeedle in pcHaystack
if not, return Null
*/
/*-----------------------------------------------------------------*/
char *StrSearch(const char* pcHaystack, const char *pcNeedle)
{
  /* TODO: fill this function */
  assert(pcHaystack);
  assert(pcNeedle);
  const char* walk = pcHaystack;
  const char* search = pcNeedle;
  const char* start = pcNeedle;
  const char* tmp ;
  enum DFAState {initial,ready,complete};
  enum DFAState state = initial;
  if(pcNeedle == '\0'){
    return (char*) walk;
  }
  if(*pcNeedle=='\0'){
    return(char*)walk;
  }
  while(*walk!= '\0'){
      switch(state){
        case(initial):
            if(*walk == *search){
                state = ready;
                tmp = walk;
            }
            else {
                walk++;
            }
            break;

        case(ready):
            if(*search == '\0'){
                state = complete;
            }
            else if(*walk != *search){
                state = initial;
                search = start;
            }
            else if(*walk == *search){
                walk++;
                search++;
            }
            break;
        
        case(complete):
            break;

        }
    if(state == complete){
        return (char*) tmp;
    }
    }
    return 0;
    }

/*StrConcat*/
/* 
to concatenate the strings pcSrc to pcDest
ex) pcDest = abc, pcSrc =def, result = abcdef

parameter: the pointers to array
pcDest -> the destination of the string
pcSrc-> the source of string that client want to concatenate

return:
a pointer to the resulting string pcDest.
*/

/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest);
  assert(pcSrc);
  char* start = pcDest;
  while(*start != '\0'){
    start ++;
  }
  while(*pcSrc !='\0'){
    *start = *pcSrc ;
    pcSrc ++;
    start ++;
  }
  *start = '\0'; 
  return pcDest;
}