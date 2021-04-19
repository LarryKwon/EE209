#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024

struct UserInfo
{
  char *name;   // customer name
  char *id;     // customer id
  int purchase; // purchase amount (> 0)
};

struct DB
{
  struct UserInfo *pArray; // pointer to the array
  int curArrSize;          // current array size (max # of elements)
  int numItems;            // # of stored items, needed to determine
                           // # whether the array should be expanded
                           // # or not
};

/*--------------------------------------------------------------------*/
DB_T CreateCustomerDB(void)
{
  DB_T d;

  d = (DB_T)calloc(1, sizeof(struct DB));
  if (d == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize, sizeof(struct UserInfo));
  if (d->pArray == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n", d->curArrSize);
    free(d);
    return NULL;
  }
  return d;
}
/*--------------------------------------------------------------------*/
/*
should free all memory occupied by the DB_T object and all the underlying objects.
If the given pointer to the DB_T object is NULL, it should do nothing.
*/

void DestroyCustomerDB(DB_T d)
{
  //validate the paramter

  if (d == NULL)
  {
  }
  else
  {
    // assert(d);

    struct UserInfo *node;
    struct UserInfo *next;
    int i;
    for (int i = 0; i < d->curArrSize; i++)
    {

      free(d->pArray[i].name);
      d->pArray[i].name = NULL;
      free(d->pArray[i].id);
      d->pArray[i].id = NULL;
    }
    free(d->pArray);
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
  // assert(d);
  // assert(id);
  // assert(name);
  // assert(purchase > 0);

  for (size_t i = 0; i < (d->curArrSize); i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    if (strcmp(d->pArray[i].id, id) == 0)
    {
      return (-1);
    }
    else if (strcmp(d->pArray[i].name, name) == 0)
    {
      return (-1);
    }
  }
  int i;
  for (i = 0; d->pArray[i].name != NULL; i++)
  {
  }
  if ((d->pArray[i].id = strdup(id)) == NULL)
  {
    return (-1);
  };
  if ((d->pArray[i].name = strdup(name)) == NULL)
  {
    return (-1);
  };
  d->pArray[i].purchase = purchase;
  return 0;
}
/*--------------------------------------------------------------------*/
/*
On success, it should return 0. Otherwise, it should return -1.
If d or id is NULL, it is a failure. If no such item exists, it is a failure.
Make sure that you free all the memory allocated for the item being unregistered.
*/
int UnregisterCustomerByID(DB_T d, const char *id)
{
  //validate paramter
  if (d == NULL || id == NULL)
  {
    return (-1);
  }
  // assert(d);
  // assert(id);

  //search item in db//
  for (int i = 0; i < d->curArrSize; i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    if (strcmp(d->pArray[i].id, id) == 0)
    {
      free(d->pArray[i].name);
      d->pArray[i].name = NULL;

      free(d->pArray[i].id);
      d->pArray[i].id = NULL;

      return 0;
    }
  }
  return (-1);
  //unregister//
}

/*--------------------------------------------------------------------*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
  //validate paramter
  if (d == NULL || name == NULL)
  {
    return (-1);
  }
  // assert(d);
  // assert(name);

  //search item in db//
  for (int i = 0; i < d->curArrSize; i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    if (strcmp(d->pArray[i].name, name) == 0)
    {
      free(d->pArray[i].name);
      d->pArray[i].name = NULL;

      free(d->pArray[i].id);
      d->pArray[i].id = NULL;

      return 0;
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetPurchaseByID(DB_T d, const char *id)
{
  //validate paramter
  if (d == NULL || id == NULL)
  {
    return (-1);
  }

  //get method
  for (int i = 0; i < d->curArrSize; i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    if (strcmp(d->pArray[i].id, id) == 0)
    {
      return d->pArray[i].purchase;
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetPurchaseByName(DB_T d, const char *name)
{

  //validate paramter
  if (d == NULL || name == NULL)
  {
    return (-1);
  }

  //get method
  for (int i = 0; i < d->curArrSize; i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    if (strcmp(d->pArray[i].name, name) == 0)
    {
      return d->pArray[i].purchase;
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  //validate paramter
  if (d == NULL || fp == NULL)
  {
    return (-1);
  }
  int sum = 0;
  for (int i = 0; i < d->curArrSize; i++)
  {
    if (d->pArray[i].name == NULL)
    {
      continue;
    }
    else
    {
      sum += (*fp)(d->pArray[i].id, d->pArray[i].name, d->pArray[i].purchase);
    }
  }
  return sum;
}