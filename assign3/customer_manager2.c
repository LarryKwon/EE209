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
};

const typedef struct UserInfo *UserInfoPtr;

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
  }

  // allocate memory for d->ht_name
  d->ht_name = (UserInfoPtr *)calloc(BUCKET_COUNT, sizeof(UserInfoPtr));
  if (d->ht_name == NULL)
  {
    fprintf(stderr, "Can't allocate a memory for ht_name\n");
  }
  d->curArrSize = BUCKET_COUNT;

  return NULL;
}
/*--------------------------------------------------------------------*/
void DestroyCustomerDB(DB_T d)
{

  assert(0);
  return NULL;
}
/*--------------------------------------------------------------------*/
int RegisterCustomer(DB_T d, const char *id,
                     const char *name, const int purchase)
{
  /* fill out this function */
  assert(0);
  return (-1);
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