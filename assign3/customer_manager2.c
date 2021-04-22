#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define BUCKET_COUNT 1024

enum
{
  HASH_MULTIPLIER = 65599
};

/*------------------------------------------------------------------*/
/* 
Return a hash code for pcKey that is between 0 and iBucketCount-1,
inclusive. Adapted from the EE209 lecture notes. 
*/
static int hash_function(const char *pcKey, int iBucketCount)
{
  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER + (unsigned int)pcKey[i];
  return (int)(uiHash % (unsigned int)iBucketCount);
}

/*------------------------------------------------------------------*/

struct UserInfo
{
  char *name;   // customer name
  char *id;     // customer id
  int purchase; // purchase amount (> 0)
  struct UserInfo *nextId;
  struct UserInfo *nextName;
};
typedef struct UserInfo *UserInfoPtr;

struct DB
{
  UserInfoPtr *ht_id;
  UserInfoPtr *ht_name;
  int numItems;
  int curArrSize;
};

/*------------------------------------------------------------------*/
DB_T CreateCustomerDB(void)
{
  //allocate memory for d
  DB_T d;
  d = (DB_T)calloc(1, sizeof(struct DB));
  if (d == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }

  // allocate memory for d->ht_id
  d->ht_id = (UserInfoPtr *)calloc(BUCKET_COUNT, sizeof(UserInfoPtr));
  if (d->ht_id == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for ht_id\n");
    return NULL;
  }

  // allocate memory for d->ht_name
  d->ht_name = (UserInfoPtr *)calloc(BUCKET_COUNT, sizeof(UserInfoPtr));
  if (d->ht_name == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for ht_name\n");
    return NULL;
  }
  d->curArrSize = BUCKET_COUNT;

  return d;
}
/*--------------------------------------------------------------------*/
/*
should free all memory occupied by the DB_T object and all the underlying objects.
If the given pointer to the DB_T object is NULL, it should do nothing.
*/
void DestroyCustomerDB(DB_T d)
{
  if (d == NULL)
  {
  }
  else
  {
    UserInfoPtr ptr = NULL;
    UserInfoPtr nextPtr = NULL;
    for (int i = 0; i < d->curArrSize; i++)
    {
      for (ptr = d->ht_id[i]; ptr != NULL; ptr = nextPtr)
      {
        nextPtr = ptr->nextId;
        free(ptr->id);
        free(ptr->name);
        ptr->id = NULL;
        ptr->name = NULL;
        ptr->purchase = 0;

        ptr->nextId = NULL;
        ptr->nextName = NULL;

        free(ptr);
      }
    }
    free(d->ht_id);
    free(d->ht_name);
    free(d);
  }
}
/*--------------------------------------------------------------------*/
/*
store a new user item with her customer information with d.
The new item should _own_ the id and name strings by copying them to new buffers. Consider using strdup(). 
If you use strdup(), please add -D_GNU_SOURCE after gcc209. (e.g., $ gcc209 -D_GNU_SOURCE -o testclient testclient.c)

On success, it should return 0. Otherwise (e.g., on failure), it should return -1.
If any of d, id, or name is NULL, it is a failure. 
If purchase is zero or a negative number, it is a failure.
If an item with the same id _or_ with the same name already exists, it is a failure. 
You should not modify the existing item in this case, and should not leak memory for the new item. 
A good strategy is to check if such an item already exists, and allocate the new item only if it does not exist
*/
int RegisterCustomer(DB_T d, const char *id, const char *name, const int purchase)
{
  // validate the paramter
  if (d == NULL || id == NULL || name == NULL || purchase <= 0)
  {
    return (-1);
  }

  // construct UserInfo
  UserInfoPtr newUser = (UserInfoPtr)calloc(1, sizeof(struct UserInfo));
  if (newUser == NULL)
  {
    return (-1);
  }
  if ((newUser->id = strdup(id)) == NULL)
  {
    return (-1);
  };
  if ((newUser->name = strdup(name)) == NULL)
  {
    return (-1);
  };
  newUser->purchase = purchase;

  // Create hashName, hashId
  int hashName = hash_function(newUser->name, d->curArrSize);
  int hashId = hash_function(newUser->id, d->curArrSize);

  // check whether there is same thing in ht_id or ht_name
  UserInfoPtr ptr = NULL;
  UserInfoPtr nextPtr = NULL;
  for (ptr = d->ht_id[hashId]; ptr != NULL; ptr = nextPtr)
  {
    if (strcmp(ptr->id, newUser->id) == 0)
    {
      free(newUser->name);
      free(newUser->id);
      free(newUser);
      return (-1);
    }
    nextPtr = ptr->nextId;
  }

  ptr = NULL;
  nextPtr = NULL;
  for (ptr = d->ht_name[hashName]; ptr != NULL; ptr = nextPtr)
  {
    if (strcmp(ptr->name, newUser->name) == 0)
    {
      free(newUser->name);
      free(newUser->id);
      free(newUser);
      return (-1);
    }
    nextPtr = ptr->nextName;
  }

  // linking previous Node to new Node
  newUser->nextId = d->ht_id[hashId];
  newUser->nextName = d->ht_name[hashName];

  // link new Node to hashTables
  d->ht_id[hashId] = newUser;
  d->ht_name[hashName] = newUser;

  //increment d->numItems
  d->numItems++;

  return 0;
}
/*--------------------------------------------------------------------*/
int UnregisterCustomerByID(DB_T d, const char *id)
{
  /* fill out this function */
  assert(0);
  return (-1);
}

/*--------------------------------------------------------------------*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
  /* fill out this function */
  assert(0);
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetPurchaseByID(DB_T d, const char *id)
{
  /* fill out this function */
  assert(0);
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetPurchaseByName(DB_T d, const char *name)
{
  /* fill out this function */
  assert(0);
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* fill out this function */
  assert(0);
  return (-1);
}