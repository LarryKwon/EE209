/* 권혁태 20180036 assignment 2 , sgrep.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "str.h"

#define FIND_STR        "-f"
#define REPLACE_STR     "-r"
#define DIFF_STR        "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

typedef enum {
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;

/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void 
PrintUsage(const char* argv0) 
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

/* Description
function: as descripted above

parameter: the source that we want to search
pcSearch -> string that we want to search
return: if it has problem false, else true
*/

/*-------------------------------------------------------------------*/
int
DoFind(const char *pcSearch)
{
  char buf[MAX_STR_LEN + 2]; 
  int len;
  //error case: pcSearch is null
  if(pcSearch=='\0'){
    fprintf(stderr,"Error: Can't find an empty Substring\n");
  }
  // find
  while (fgets(buf, sizeof(buf), stdin)) {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    const char* tmp = (const char*) buf;
    if(StrSearch(tmp,pcSearch)!=NULL){
      printf("%s",tmp);
    }
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

/* Description
function: as descripted above

parameter: the source of replacement
pcString1 -> the pointer of the string that will be replaced
pcString2-> the pointer of string that will replace the pcString2
return: if it has problem false, else true
*/
/*-------------------------------------------------------------------*/

int
DoReplace(const char *pcString1, const char *pcString2)
{
  char buf[MAX_STR_LEN + 2]; 
  int len;
  size_t s1_len = StrGetLength(pcString1);
  size_t s2_len = StrGetLength(pcString2);

  /* check the pcString1 is empty Substrin */
  if(pcString1=='\0'){
  fprintf(stderr,"Error: Can't replace an empty substring\n");
  return FALSE;
  }

  while (fgets(buf, sizeof(buf), stdin)) {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }

    /* walk will traverse the buf */
    /*
    if the pcString1 is found in buf,
    1. cut the buf after the pcString1's end pointer
    and copy to the array new
    2. copy the pcString2 to walk
    3. concatenate the new to the end of pcString2 
    */
    char* walk = buf;
    while(StrSearch(walk,pcString1)!=0){
      char new[1024];
      walk = StrSearch(walk,pcString1);
      StrCopy(new,walk+s1_len);
      StrCopy(walk,pcString2);
      walk = walk + s2_len;
      StrConcat(walk,new);
    }
    printf("%s",buf);
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
      error message "
      Error: [filename] ends early at line XX", where
      XX is the final line number of [filename].

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */

/* Description
function: as descripted above

parameter: check source, two file's name
return: if it has problem false, else true
*/
/*-------------------------------------------------------------------*/
int
DoDiff(const char *file1, const char *file2)
{

  int len1;
  int len2;
  /* check the name of file length */
  if(StrGetLength(file1)>1023 || StrGetLength(file2)>1023){
    fprintf(stderr,"Error: arugment is too long");
    return FALSE;
  }
  FILE *f1;
  FILE *f2;
  f1 = fopen(file1,"r");
  f2 = fopen(file2,"r");
  int count = 0;
  char* res1;
  char* res2;
  // if f1 can't be opened//
  if(f1==NULL){
    fprintf(stderr,"Error: failed to open file %s\n",file1);
    return FALSE;
  }
  // if f2 can't be opened//
  else if(f2==NULL){
    fprintf(stderr,"Error: failed to open file %s\n",file2);
    return FALSE;
  }
  else{
    //well opened//
    //read each line//
    while (1){
    char buf_f1[MAX_STR_LEN + 2]="\0"; 
    char buf_f2[MAX_STR_LEN+2]="\0";
    res1=fgets(buf_f1, sizeof(buf_f1), f1);
    res2=fgets(buf_f2,sizeof(buf_f2),f2);

    if(res1 =='\0' || res2 == '\0'){
      break;
    }
    /* check input line length */
    if ((len1 = StrGetLength(buf_f1)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line %s is too long\n",
      file1);
      return FALSE;
    }
    else if((len2 = StrGetLength(buf_f2)) > MAX_STR_LEN ){
      fprintf(stderr, "Error: input line %s is too long\n",
      file2);
      return FALSE;
    }
    count++;


    if(buf_f1[StrGetLength(buf_f1)-1]!='\n'){
      buf_f1[StrGetLength(buf_f1)]='\n';
      buf_f1[StrGetLength(buf_f1)]='\0';
    }
    if(buf_f2[StrGetLength(buf_f2)-1]!='\n'){
      buf_f2[StrGetLength(buf_f2)]='\n';
      buf_f2[StrGetLength(buf_f2)]='\0';
    } 
    //compare each line//
    if( StrCompare(buf_f1,buf_f2) !=0){
      printf("%s@%d:%s",file1,count,buf_f1);
      printf("%s@%d:%s",file2,count,buf_f2);
    }

    }
    if(res1=='\0' && res2 !='\0'){
        fprintf(stderr,"Error: %s ends early at line %d\n",
        file1,count);
        return FALSE;
        }
    else if(res2=='\0' && res1 != '\0'){
        fprintf(stderr,"Error: %s ends early at line %d\n",
        file2,count);
        return FALSE;
    }
  }
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
int
CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;
   
  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;
   
  /* check command type */ 
  if (strcmp(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (strcmp(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (strcmp(argv1, DIFF_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }
   
  return cmdtype;
}
/*-------------------------------------------------------------------*/
int 
main(const int argc, const char *argv[]) 
{
  int type, ret;
   
  /* Do argument check and parsing */
  if (!(type = CommandCheck(argc, argv[1]))) {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }
   
  /* Do appropriate job */
  switch (type) {
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

  return (ret)? EXIT_SUCCESS : EXIT_FAILURE;
}