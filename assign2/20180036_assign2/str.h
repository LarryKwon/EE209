#ifndef _STR_H_
#define _STR_H_
#include <unistd.h> /* for typedef of size_t */

/* Part 1 */
/* StrGetLength */
size_t StrGetLength(const char* pcSrc);
/* 
to Get Length of string in the array 
parameter: the pointer to array
return : the size of array 
*/

/*StrCopy*/
char *StrCopy(char *pcDest, const char* pcSrc);
/* 
to copy the string in the array
parameter: the pointer to array
pcSrc -> pointer of array copy (origin)
pcDest -> pointer of array copy (destination)
return: the start of destinationi array
*/

/*StrCompare*/
int StrCompare(const char* pcS1, const char* pcS2);
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


/*StrSearch*/
char *StrSearch(const char* pcHaystack, const char *pcNeedle);
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

/*StrConcat*/
char *StrConcat(char *pcDest, const char* pcSrc);
/* 
to concatenate the strings pcSrc to pcDest
ex) pcDest = abc, pcSrc =def, result = abcdef

parameter: the pointers to array
pcDest -> the destination of the string
pcSrc-> the source of string that client want to concatenate

return:
a pointer to the resulting string pcDest.
*/


#endif /* _STR_H_ */