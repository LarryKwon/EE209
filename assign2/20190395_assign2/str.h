/* Ohjihwan Assignment2 str.h */
#ifndef _STR_H_
#define _STR_H_
#include <unistd.h> /* for typedef of size_t */

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
size_t StrGetLength(const char* pcSrc);
/*------------------------------------------------------------------------*/
/* Function : StrCopy()
   -------------------------------------------------------------------------
   This function receive two pointer, as pcDest and pcSrc
   Function copy the pcSrc to pcDest and return the pointer
   which point the starting location of pcDest.
   
   return : copied(pointer that point the starting location of pcDest)
   
 */
char *StrCopy(char *pcDest, const char* pcSrc);
/*------------------------------------------------------------------------*/
/* Function : StrCompare()
   -------------------------------------------------------------------
   This function receive two string as pcS1 and pcS2
   Function compares two string and return the compare result

   return :
   if two string is same,
   return ==> 0
   if two string is different,
   return ==> *pcS1 - *pcS2
   (the difference of ASCII value between pcS1's character
   and pcS2's character(i.e *pcS1 - *pcS2) which located in the location
   that two string be different.)

*/
int StrCompare(const char* pcS1, const char* pcS2);
/*------------------------------------------------------------------------*/
/* Function : StrSearch()
   -------------------------------------------------------
   This function receive two string as pcHaystack and pcNeedle
   Function check whether the pcNeedle exist in pcHaystack or not.
   
   return :
   if pcNeedle is NULL,
   return ==> pcHaystack (without change)
   if pcNeedle is exist in pcHaystack,
   return ==> pcHaystack (which point the starting location of pcNeedle
                          in pcHaystack)
   if pcNeedle is not exist in pcHaystack,
   return ==> NULL
 */
char *StrSearch(const char* pcHaystack, const char *pcNeedle);
/*------------------------------------------------------------------------*/
/* Function : StrConcat()
   -----------------------------------------------------------------------
   This function receive two string, as pcDest and pcSrc
   Function merge two string, pcDest and pcSrc, by moving the character
   sequence of pcSrc at the end of pcDest

   return : merge(store the address of staring point of pcDest)
 */

char *StrConcat(char *pcDest, const char* pcSrc);
/*------------------------------------------------------------------------*/
/* Function : hay_ned_compare()
   -------------------------------------------------------------------
   This funciton receives two string, as pcHaystack and pcNeedle, and
   check whether the pcNeedle and pcHaystack have the same character
   sequence
      
   return:
   if pcNeedle and pcHaystack have the same character sequence
   ==> return enter_ad
   if pcNeedle and pcHaystack have the different character sequence
   ==> return pcHaystack
 */
char *hay_ned_compare(const char* pcHaystack, const char* pcNeedle);

#endif /* _STR_H_ */
