#include <assert.h> /* to use assert() */
#include <stdio.h>
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
size_t StrGetLength(const char *pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;

  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char *pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest != NULL);
  assert(pcSrc != NULL);

  const char *walk = pcSrc;
  char *dest = pcDest;
  while (*walk != '\0')
  {
    *pcDest = *walk;
    walk += 1;
    pcDest += 1;
  }
  *pcDest = '\0';
  // printf("%s",dest);
  return dest;
}

/*------------------------------------------------------------------------*/
int StrCompare(const char *pcS1, const char *pcS2)
{
  /* TODO: fill this function */
  assert(pcS1 != NULL);
  assert(pcS2 != NULL);

  char *src1 = (char *)pcS1;
  char *src2 = (char *)pcS2;
  while (*src1 != '\0' || *src2 != '\0')
  {
    if (*src1 != *src2)
    {
      return (*src1 - *src2);
    }
    src1++;
    src2++;
  }
  return 0;
}
/*------------------------------------------------------------------------*/
char *StrSearch(const char *pcHaystack, const char *pcNeedle)
{
  /* TODO: fill this function */
  assert(pcHaystack != NULL);
  assert(pcNeedle != NULL);

  size_t needleLength = StrGetLength(pcNeedle);
  char *needle = (char *)pcNeedle;
  int i = 0;
  char *startPoint = NULL;

  while (*(pcHaystack + i) != '\0')
  {
    // printf("%c", * (pcHaystack + i));
    if (*needle == '\0')
    {
      startPoint = (char *)pcHaystack + (i - needleLength);
      break;
    }
    if (*(pcHaystack + i) == *needle)
    {
      needle++;
    }
    else
    {
      needle = (char *)pcNeedle;
    }
    i++;
  }
  return startPoint;
}
/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char *pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest != NULL);
  assert(pcSrc != NULL);

  char *startPoint = pcDest;
  char *endPoint = (char *)pcSrc;

  while (*startPoint != '\0')
  {
    startPoint++;
  }

  while (*endPoint != '\0')
  {
    *startPoint = *endPoint;
    startPoint++;
    endPoint++;
  }
  *startPoint = '\0';
  return pcDest;
}