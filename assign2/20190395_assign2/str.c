/* OhjiHwan assignment2 str.c*/
#include <assert.h> /* to use assert() */
#include <stdio.h>
#include "str.h"
//#include <string.h>
#include <stdlib.h>

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
/* Function : StrGetLength()
   -----------------------------------------------------------------------
   This function receive the string, as pcSrc, and return the length of
   string. The value of pcEnd is increased until pcEnd point the
   null character.
   After stop increasing, calculate the difference of address, and it
   mean the length of string, exclude the null character.
   
   pcEnd : pointer, using for find the address of null character

   return : the length of string
*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
/* Function : StrCopy()
   -------------------------------------------------------------------------
   This function receive two pointer, as pcDest and pcSrc
   Function copy the pcSrc to pcDest and return the pointer
   which point the starting location of pcDest.

   copied : pointer which point the starting location of pcDest
   
   return : copied
   
 */
char *StrCopy(char *pcDest, const char* pcSrc)
{
  /*
  if(*pcSrc == '\0'){
    *pcDest = '\0';
    return pcDest;
    }
  */
  assert(pcDest);
  assert(pcSrc);
  char* copied;
  copied = pcDest;
  /* copy pcSrc to pcDest */
  while(*pcSrc != '\0'){ 
    *pcDest = *pcSrc;
    pcDest++;
    pcSrc++;
  }
  
  *pcDest = '\0';

  return copied;
}

/*------------------------------------------------------------------------*/
/* Function : StrCompare()
   -------------------------------------------------------------------
   This function receive two string as pcS1 and pcS2
   Function compares two string and return the compare result
   
   len_pcS1 = store the length of pcS1
   len_pcS2 = store the length of pcS2
   max_len = store the length of larger string between pcS1 and pcS2

   return :
   if two string is same,
   return ==> 0
   if two string is different,
   return ==> *pcS1 - *pcS2
   (the difference of ASCII value between pcS1's character
   and pcS2's character(i.e *pcS1 - *pcS2) which located in the location
   that two string be different.)

*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  assert(pcS1);
  assert(pcS2);
  size_t len_pcS1 = StrGetLength(pcS1);
  size_t len_pcS2 = StrGetLength(pcS2);
  size_t max_len = 0;
  /*
  if(len_pcS1 != len_pcS2){
    return (int)(len_pcS1-len_pcS2);
  }
  */
  /* find the longer length between pcS1 and pcS2 */
  max_len = len_pcS1;
  if(len_pcS2 > len_pcS1){
    max_len = len_pcS2;
  }
  /* compare two string pcS1 and pcS2 */
  for(size_t i = 0; i<max_len; i++){
    /* if they are different, return *pcS1 - *pcS2 */
    if(*pcS1 != *pcS2){
      
      return (int)(*pcS1-*pcS2);
    }
    pcS1++;
    pcS2++;
  }
  return 0;
}
/*------------------------------------------------------------------------*/
/* Function : StrSearch()
   -------------------------------------------------------
   This function receive two string as pcHaystack and pcNeedle
   Function check whether the pcNeedle exist in pcHaystack or not.
   
   len_hay : store the length of pcHaystack
   len_ned : store the length of pcNeedle
   enter_ad : store the address of pcHaystack when the character of
              pcHaystack and the first character of pcNeedle
   
   return :
   if pcNeedle is NULL,
   return ==> pcHaystack (without change)
   if pcNeedle is exist in pcHaystack,
   return ==> pcHaystack (which point the starting location of pcNeedle
                          in pcHaystack)
   if pcNeedle is not exist in pcHaystack,
   return ==> NULL
 */
char *StrSearch(const char* pcHaystack, const char *pcNeedle)
{
  assert(pcHaystack);
  assert(pcNeedle);
  if(*pcNeedle == '\0'){
    return (char*)pcHaystack;
  }
  size_t len_hay = StrGetLength(pcHaystack);
  const size_t len_ned = StrGetLength(pcNeedle);
  /* if len_hay is smaller than len_ned, it is impossible that
     pcNeedle is exist in pcHaystack
   */
  while(len_hay >= len_ned){
    /* if the first character of pcNeedle is equal to the character of
       pcHaystack, check whether the rest part is same too.
 */
    if(*pcHaystack == *pcNeedle){
      const char *enter_ad = pcHaystack;
      pcHaystack = hay_ned_compare(pcHaystack, pcNeedle);
      /* enter_ad == pcHaystack mean pcNeedle is exist in pcHaystack */
      if(enter_ad == pcHaystack){
  	return (char*)pcHaystack;
      }
      
      /* we need to check whether the character that make difference is
	 the first character of pcNeedle. So we decrease the value of
	 pcHaystack
      */
      pcHaystack--;
    }
    pcHaystack++;
    len_hay = StrGetLength(pcHaystack);
  }
 
  return (char*)'\0';
}
/*------------------------------------------------------------------------*/
/* Function : StrConcat()
   -----------------------------------------------------------------------
   This function receive two string, as pcDest and pcSrc
   Function merge two string, pcDest and pcSrc, by moving the character
   sequence of pcSrc at the end of pcDest

   merge : store the address of staring point of pcDest

   return : merge
 */
char *StrConcat(char *pcDest, const char* pcSrc)
{
  char* merge;
  merge = pcDest;
  /* To merge it at the end of pcDest,
     move the pointer to the end of pcDest
   */
  while(*pcDest != '\0'){
    pcDest++;
  }
  /* merge the pcSrc to pcDest at the end of pcDest */
  while(*pcSrc != '\0'){
    *pcDest = *pcSrc;
    pcDest++;
    pcSrc++;
  }
  *pcDest = '\0';
  
  return merge;
}
/*------------------------------------------------------------------------*/
/* Function : hay_ned_compare()
   -------------------------------------------------------------------
   This funciton receives two string, as pcHaystack and pcNeedle, and
   check whether the pcNeedle and pcHaystack have the same character
   sequence
   
   enter_ad : store the address of pcHaystack,
              when the function start running
   
   return:
   if pcNeedle and pcHaystack have the same character sequence
   ==> return enter_ad
   if pcNeedle and pcHaystack have the different character sequence
   ==> return pcHaystack
 */
char *hay_ned_compare(const char *pcHaystack, const char *pcNeedle){
  const char *enter_ad;
  enter_ad = pcHaystack;
  while(*pcNeedle != '\0'){
    if(*pcHaystack != *pcNeedle){
      return (char*)pcHaystack;
    }
    pcHaystack++;
    pcNeedle++;
  }
  return (char*)enter_ad;
}
