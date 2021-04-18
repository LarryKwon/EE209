/* Ohjihwan Assignment2 sgrep.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
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

/* --------------------------------------------------------------------
   Function : print_until_str1
   --------------------------------------------------------------------
   This Function receive three string, temp, str1, str2
   Function print the temp until str1 comes.
   After print the temp until str1 comes, print the str2
   
   len_str2 : store the length of str2
   index : store the distance between the first location of temp
           and str1
   
   return : temp
 */
char*print_until_str1(char*temp,const char*str1,const char*str2){
  size_t len_str2 = StrGetLength(str2);
  int index = StrSearch(temp, str1) - temp;
  /* print character until the str1 come  */
  for(int j = 0; j<index; j++){
    printf("%c",*temp);
    temp++;
  }
  /* after for loop temp point the first character of str1  */
  /* print str2 at the location of str1 existed */
  for(int k = 0; k<len_str2; k++){
    printf("%c", *str2);
    str2++;
  }
  return temp;
}

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
/* -------------------------------------------------------------------
   Function : DoFind()
   -------------------------------------------------------------------
   This function receive the string, as pcSearch that we want
   to find at stdin. Function check the existance of the pcSearch
   at each line of stdin, and if the pcSearch exist,
   print the line that pcSearch is included as stdout.
   Also we need to check the length of argument and each line from
   stdin is bigger than 1023 bytes, if they have bigger length than
   1023 bytes, make a error as stderr
   
   buf : store the each line from stdin
   len : store the length of line from stdin
   len_search : store the length of comment-argument
   exist : store the return value of StrSearch

   return :
   if error occur,
   return ==> FALSE
   if error not occur,
   return ==> TRUE

 */
int
DoFind(const char *pcSearch)
{
  char buf[MAX_STR_LEN + 2]; 
  int len;
  int len_search;
  
  /* check command-line argument line length */
  if((len_search = StrGetLength(pcSearch)) > MAX_STR_LEN){
   fprintf(stderr,"Error: argument is too long\n");
   return FALSE;
    
  }
  while (fgets(buf, sizeof(buf), stdin)) {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    /* if pcSearch is null, print the all line of stdin */
    if(*pcSearch == '\0'){
      
      printf("%s",buf);
      fflush(stdout);
      continue;
    }
    
    char* exist;
    
    exist = StrSearch(buf, pcSearch);
    /* if exist is null, there are no pcSearch at buf.
       Adversely, if exist is not null, there are pcSearch at line
       and print that line as stdout.
     */
    if( exist != (char*)'\0'){
      printf("%s",buf);
      fflush(stdout);
    }
    
  }
   
  return TRUE;
}

/* -------------------------------------------------------------------
   Function : DoReplace()
   -------------------------------------------------------------------
   This function receive the two strings, as pcString1, pcString2.
   Function get the each line from stdin, and print the line
   which is replaced all of the pcString1 that exist at line
   from stdin, to pcString2 as stdout.
   Also we need to check the length of argument and each line from
   stdin is bigger than 1023 bytes, if they have bigger length than
   1023 bytes, make an error as stderr.
   In addition, if the pcString1 is empty string, we also need to
   make an error as stderr.
   
   len_pc1 : store the length of pcString1
   len_pc2 : store the length of pcString2
   buf : store the each line from stdin
   len : store the length of line from stdin
   temp : copy buf to find the number of pcString1 in input line
   rest : copy buf to print the changed input line
   
   return :
   if error occur,
   return ==> FALSE
   if error not occur,
   return ==> TRUE
   
 */


int
DoReplace(const char *pcString1, const char *pcString2)
{
  size_t len_pc1 = StrGetLength(pcString1);
  size_t len_pc2 = StrGetLength(pcString2);
  /* check the argument length */
  if(len_pc1 > MAX_STR_LEN || len_pc2 > MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
      return FALSE;
  }
  char buf[MAX_STR_LEN +2];
  int len;
  char *temp;
  while (fgets(buf, sizeof(buf), stdin)) {
  /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    /* check input line, whether it is empty string or not */
    if(*pcString1 == '\0'){
      fprintf(stderr, "Error: Can't replace an empty substring\n");
      return FALSE;
    }
    int cnt = 0;
    /* initialize the temp */
    temp = buf;
    /* count the number of pcString1 in rest */
    while(StrSearch(temp,pcString1) != (char*)'\0'){
      temp = StrSearch(temp,pcString1);
      cnt++;
      /* move pointer to behind of str1 */
      for(int j = 0; j<len_pc1; j++){
	temp++;
      }
    }
    
    char*rest;
    /* initialize the rest */
    rest = buf;
    /* print the changed line as stdout */
    for(int i =0; i<cnt; i++){
      rest=print_until_str1(rest,pcString1,pcString2);
      /* 
	 change rest value to make it
	 pointing the behind of str1
      */
      for(int k = 0; k<len_pc1; k++){
	rest++; 
      }
    }
    /* print rest part of line
       (behind part of pcString2)
    */
    printf("%s", rest);
    }
     
  return TRUE;
}

/* -------------------------------------------------------------------
   Function : DoDiff()
   -------------------------------------------------------------------
   This function receive the two strings, as file1, file2.
   Function get the each line from the file which names are file1,
   file2. And it compare the two line each line from each file.
   If the two lines are different we print the line with its
   line number and print it to stdout.
   Also we need to check the length of argument from stidn
   and each line from files is bigger than 1023 bytes,
   if they have bigger length than 1023 bytes,
   make an error as stderr.   
   In addition, if we can't open the file, we need to make an error
   as stderr. Also, if one file is closed faster than other file,
   than we also need to make an error as stderr.
   
   len_f1_name : store the name length of file1
   len_f2_name : store the name length of file2
   f1 : store the file1 pointer
   f2 : store the file2 pointer
   line_cnt : store line number
   ret_f1 : store return value of fgets with f1
   ret_f2 : store return value of fgets with f2
   ar_line_f1 : store the each line from file1
   ar_line_f2 : store the each line from file2
   len_f1 : store the line length from file1
   len_f2 : store the line length from file2
   
   return :
   if error occur,
   return ==> FALSE
   if error not occur,
   return ==> TRUE
   
 */

int
DoDiff(const char *file1, const char *file2)
{
  size_t len_f1_name = StrGetLength(file1);
  size_t len_f2_name = StrGetLength(file2);
  /* check command line argument's length */
  if(len_f1_name > MAX_STR_LEN || len_f2_name > MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
  }

  FILE* f1;
  FILE* f2;
  f1 = fopen(file1, "r");
  f2 = fopen(file2, "r");
  
  /* check whether the file is opened clearly */
  if(f1 == NULL){
    fprintf(stderr, "Error : failed to open file [%s]\n", file1);
    fclose(f1);
    fclose(f2);
    
    return FALSE;
  }
  if(f2 == NULL){
    fprintf(stderr, "Error : failed to open file [%s]\n", file2);
    fclose(f1);
    fclose(f2);
    
    return FALSE;
  }

   
  int line_cnt = 0;	  
  char*ret_f1; 		  
  char*ret_f2;		  

  while(TRUE){
    char ar_line_f1[MAX_STR_LEN+2] = "";
    char ar_line_f2[MAX_STR_LEN+2] = "";

    ret_f1 = fgets(ar_line_f1, MAX_STR_LEN+2, f1);
    ret_f2 = fgets(ar_line_f2, MAX_STR_LEN+2, f2);
    /* count line number */
    line_cnt++;
    
    if(ret_f1 == NULL || ret_f2 == NULL){
      break;
    }
    
    int len_f1 = StrGetLength(ar_line_f1);
    int len_f2 = StrGetLength(ar_line_f2);
    /* check encounter line's length */
    if(len_f1 > MAX_STR_LEN){
      fprintf(stderr, "Error : input line %s is too long\n",
	      file1);
      fclose(f1);
      fclose(f2);
      return FALSE;
    }
    if(len_f2 > MAX_STR_LEN){
      fprintf(stderr, "Error : input line %s is too long\n",
	      file2);
      fclose(f1);
      fclose(f2);
      return FALSE;
    }
    /* all lines except the last line must end with '\n'
       and if the last line does not have '\n' at the end,
       they should be considered as if it has '\n' at
       the end, in out sgrep.
       so check whether the line have '\n' at the end,
       and if it doesn't have we need to add '\n'
       at the end.
    */
    if(ar_line_f1[len_f1-1] != '\n'){
      ar_line_f1[len_f1] = '\n';
      ar_line_f1[len_f1+1] = '\0';
      len_f1++;
    }
    if(ar_line_f2[len_f2-1] != '\n'){
      ar_line_f2[len_f2] = '\n';
      ar_line_f2[len_f2+1] = '\0';
      len_f2++;
    }
    /* compare each character, and if they are different,
       print that line with file name and line number.
     */
    if(StrCompare(ar_line_f1, ar_line_f2) != 0){
      printf("%s@%d:%s", file1,line_cnt,ar_line_f1);
      printf("%s@%d:%s", file2,line_cnt,ar_line_f2);	
    }
    
  }
  if(ret_f1 != NULL){ 		/* it mean file2 ends early */
    fprintf(stderr, "Error: %s ends early at line %d\n",
	    file2, line_cnt-1);
    fclose(f1);
    fclose(f2);
    return FALSE;
  }
  
  if(ret_f2 != NULL){ 		/* it mean file1 ends early */
    fprintf(stderr, "Error: %s ends early at line %d\n",
	    file1, line_cnt-1);
    fclose(f1);
    fclose(f2);
    return FALSE;
  }
  fclose(f1);
  fclose(f2);
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
  if (StrCompare(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (StrCompare(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (StrCompare(argv1, DIFF_STR) == 0) {
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
