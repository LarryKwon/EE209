#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define FIND_STR "-f"
#define REPLACE_STR "-r"
#define DIFF_STR "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE 1

typedef enum
{
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;

/*
 * Fill out your functions here (If you need) 
 */

/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void PrintUsage(const char *argv0)
{
  const static char *fmt =
      "Simple Grep (sgrep) Usage:\n"
      "%s [COMMAND] [OPTIONS]...\n"
      "\nCOMMNAD\n"
      "\tFind: -f [search-string]\n"
      "\tReplace: -r [string1] [string2]\n"
      "\tDiff: -d [file1] [file2]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* DoFind()
   Your task:
   1. Do argument validation 
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long, 
   print out "Error: argument is too long"
   
   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes, 
   print out "Error: input line is too long" 
   - Error message should be printed out to standard error (stderr)
   
   3. Check & print out the line contains a given string (search-string)
      
   Tips:
   - fgets() is an useful function to read characters from file. Note 
   that the fget() reads until newline or the end-of-file is reached. 
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int DoFind(const char *pcSearch)
{
  char buf[MAX_STR_LEN + 2];
  int len;

  /* 
   *  Fill out your variables here 
   */

  if(StrGetLength(pcSearch)>MAX_STR_LEN){
    fprintf(stderr,"Error: argument is too long\n");
    return FALSE;
  }
  /* Read the line by line from stdin, Note that this is an example */
  while (fgets(buf, sizeof(buf), stdin))
  {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN)
    {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }

    if (StrSearch(buf, pcSearch) != NULL)
    {
      printf("%s", buf);
    }
    /* TODO: fill out this function */
  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoReplace()
   Your task:
   1. Do argument validation 
      - String length is no more than 1023
      - If you encounter a command-line argument that's too long, 
        print out "Error: argument is too long"
      - If word1 is an empty string,
        print out "Error: Can't replace an empty substring"
      
   2. Read the each line from standard input (stdin)
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line is too long" 
      - Error message should be printed out to standard error (stderr)

   3. Replace the string and print out the replaced string

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int DoReplace(const char *pcString1, const char *pcString2)
{
  /* TODO: fill out this function */

  if (*pcString1 == '\0')
  {
    printf("%s", "adafd");
    fprintf(stderr, "Error: Can't replace an empty substring\n");
    return FALSE;
  }

  char buf[MAX_STR_LEN + 2];
  int len;
  int String1len = StrGetLength(pcString1);
  int String2len = StrGetLength(pcString2);

  if(String1len > MAX_STR_LEN || String2len > MAX_STR_LEN){
    fprintf(stderr,"Error: argument is too long\n");
    return FALSE;
  }

  if(*pcString1 == '\0'){
    fprintf(stderr,"Error: Can't replace an empty substring\n");
    return FALSE;
  }

  while (fgets(buf, sizeof(buf), stdin))
  {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN)
    {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    char *startPoint = StrSearch(buf, pcString1);
    char *endPoint = NULL;

    while (startPoint != NULL)
    {
      // printf("%s", buf);
      endPoint = startPoint + String1len;
      char temp[MAX_STR_LEN + 2];
      StrCopy(temp, endPoint);
      startPoint = StrCopy(startPoint, pcString2);
      startPoint = StrCopy(startPoint + String2len, temp);
      startPoint = StrSearch(startPoint, pcString1);
    }
    /*search하는 과정
    search해서 pcString1이 매치되는 지점을 찾는다. (startPoint)
    매치되는 지점으로부터 단어 길이만큼 뒤의 지점을 찾는다. (endPoint)
    startPoint로부터 pcString2만큼 덮어쓴다.
    endPoint를 이어붙인다.

    Endpoint부터 다시 검색 -> 과정 반복
    startPoint가 검색이 안될 때까지 찾는다.
    */
    printf("%s", buf);
  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoDiff()
   Your task:
   1. Do argument validation 
     - file name length is no more than 1023
     - If a command-line argument is too long, 
       print out "Error: arugment is too long" to stderr

   2. Open the two files
      - The name of files are given by two parameters
      - If you fail to open either file, print out error messsage
      - Error message: "Error: failed to open file [filename]\n"
      - Error message should be printed out to stderr

   3. Read the each line from each file
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line [filename] is too long" 
      - Error message should be printed out to stderr

   4. Compare the two files (file1, file2) line by line 

   5. Print out any different line with the following format
      file1@linenumber:file1's line
      file2@linenumber:file2's line

   6. If one of the files ends earlier than the other, print out an
      error message "Error: [filename] ends early at line XX", where
      XX is the final line number of [filename].

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int DoDiff(const char *file1, const char *file2)
{
  /* TODO: fill out this function */
  /* length of file name check */
  size_t f1_length = StrGetLength(file1);
  size_t f2_length = StrGetLength(file2);
  if (f1_length > MAX_STR_LEN || f2_length > MAX_STR_LEN)
  {
    fprintf(stderr, "Error: arugment is too long\n");
    return FALSE;
  }

  FILE *fp1 = fopen(file1, "r");
  FILE *fp2 = fopen(file2, "r");
  if (fp1 == NULL)
  {
    fprintf(stderr, "Error: failed to open file %s\n", file1);
    fclose(fp1);
    fclose(fp2);
    return FALSE;
  }
  else if (fp2 == NULL)
  {
    fprintf(stderr, "Error: failed to open file %s\n", file2);
    fclose(fp1);
    fclose(fp2);
    return FALSE;
  }
  else
  {
    char buf_f1[MAX_STR_LEN + 2];
    char buf_f2[MAX_STR_LEN + 2];
    int count = 0;
    char *res1;
    char *res2;
    while (TRUE)
    {
      res1 = fgets(buf_f1, sizeof(buf_f1), fp1);
      res2 = fgets(buf_f2, sizeof(buf_f2), fp2);

      //line exist?//
      if (res1 == 0 || res2 == 0)
      {
        break;
      }
      count++;
      int f1_len = StrGetLength(buf_f1);
      int f2_len = StrGetLength(buf_f2);

      //line size longer than 1023?//
      if (f1_len > MAX_STR_LEN)
      {
        fprintf(stderr, "Error: input line %s is too long\n", file1);
        fclose(fp1);
        fclose(fp2);
        return FALSE;
      }
      else if (f2_len > MAX_STR_LEN)
      {
        fprintf(stderr, "Error: input line %s is too long\n", file2);
        fclose(fp1);
        fclose(fp2);
        return FALSE;
      }
      /* \n exist? */
      if (buf_f1[f1_len - 1] != '\n')
      {
        buf_f1[f1_len] = '\n';
        buf_f1[f1_len + 1] = '\0';
      }
      if (buf_f2[f2_len - 1] != '\n')
      {
        buf_f2[f2_len] = '\n';
        buf_f2[f2_len + 1] = '\0';
      }

      // printf("%s\n",buf_f1);
      // printf("%s\n",buf_f2);
      //line compare//
      if (StrCompare(buf_f1, buf_f2) != 0)
      {
        // printf("%d\n",StrCompare(buf_f1,buf_f2));
        printf("%s@%d:%s", file1, count, buf_f1);
        printf("%s@%d:%s", file2, count, buf_f2);
      }
    }
    //early end?//
    if (res1 == NULL && res2 != NULL)
    {
      fprintf(stderr, "Error: %s ends early at line %d\n",
              file1, count);
      fclose(fp1);
      fclose(fp2);
      return FALSE;
    }
    else if (res2 == NULL && res1 != NULL)
    {
      fprintf(stderr, "Error: %s ends early at line %d\n",
              file2, count);
      fclose(fp1);
      fclose(fp2);
      return FALSE;
    }
  }
  fclose(fp1);
  fclose(fp2);
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* CommandCheck() 
   - Parse the command and check number of argument. 
   - It returns the command type number
   - This function only checks number of argument. 
   - If the unknown function is given or the number of argument is 
   different from required number, this function returns FALSE.
   
   Note: You SHOULD check the argument rule later                    */
/*-------------------------------------------------------------------*/
int CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;

  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;

  /* check command type */
  if (strcmp(argv1, FIND_STR) == 0)
  {
    if (argc != 3)
      return FALSE;
    cmdtype = FIND;
  }
  else if (strcmp(argv1, REPLACE_STR) == 0)
  {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (strcmp(argv1, DIFF_STR) == 0)
  {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }

  return cmdtype;
}
/*-------------------------------------------------------------------*/
int main(const int argc, const char *argv[])
{
  int type, ret;

  /* Do argument check and parsing */
  if (!(type = CommandCheck(argc, argv[1])))
  {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }

  /* Do appropriate job */
  switch (type)
  {
  case FIND:
    ret = DoFind(argv[2]);
    break;
  case REPLACE:
    ret = DoReplace(argv[2], argv[3]);
    break;
  case DIFF:
    ret = DoDiff(argv[2], argv[3]);
    break;
  }

  return (ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}