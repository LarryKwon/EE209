#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define BUCKET_COUNT 1024
#define EXPANDING_FACTOR 2
#define EXPANDING_CRITERIA 0.75
enum
{
  HASH_MULTIPLIER = 65599
};

enum HtState
{
  NORMAL,
  ENOUGH
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
  enum HtState HtIdState;
  enum HtState HtNamestate;
};

struct userChain
{
  UserInfoPtr ptr;
  UserInfoPtr prevPtr;
  UserInfoPtr nextPtr;
  int hashId;
  int hashName;
};

static struct userChain findUserById(DB_T d, const char *id)
{

  int hashId = hash_function(id, d->curArrSize);
  struct userChain userChainById;
  userChainById.prevPtr = d->ht_id[hashId];
  userChainById.ptr = NULL;
  userChainById.nextPtr = NULL;

  UserInfoPtr current = NULL;
  UserInfoPtr next = NULL;
  UserInfoPtr prev = NULL;
  for (current = d->ht_id[hashId]; current != NULL; current = next)
  {
    if (strcmp(current->id, id) == 0)
    {
      userChainById.ptr = current;
      userChainById.nextPtr = current->nextId;
      userChainById.prevPtr = prev;

      userChainById.hashId = hashId;
      userChainById.hashName = hash_function(current->name, d->curArrSize);
      break;
    }
    next = current->nextId;
    prev = current;
  }

  return userChainById;
}

/*------------------------------------------------------------------*/
static struct userChain findUserByName(DB_T d, const char *name)
{
  int hashName = hash_function(name, d->curArrSize);
  struct userChain userChainByName;
  userChainByName.prevPtr = d->ht_name[hashName];
  userChainByName.ptr = NULL;
  userChainByName.nextPtr = NULL;

  UserInfoPtr current = NULL;
  UserInfoPtr next = NULL;
  UserInfoPtr prev = NULL;
  for (current = d->ht_name[hashName]; current != NULL; current = next)
  {
    if (strcmp(current->name, name) == 0)
    {
      userChainByName.ptr = current;
      userChainByName.nextPtr = current->nextId;
      userChainByName.prevPtr = prev;

      userChainByName.hashName = hashName;
      userChainByName.hashId = hash_function(current->id, d->curArrSize);
      break;
    }
    next = current->nextId;
    prev = current;
  }

  return userChainByName;
}
/*------------------------------------------------------------------*/
static void assertSize(DB_T d)
{
  assert(d);
  if (d->numItems >= d->curArrSize * EXPANDING_CRITERIA)
  {
    d->HtIdState = ENOUGH;
    d->HtNamestate = ENOUGH;
  }
  else
  {
    d->HtIdState = NORMAL;
    d->HtNamestate = NORMAL;
  }
}

static void ResizingHtId(DB_T d)
{

  assertSize(d);
  switch (d->HtIdState)
  {
  case ENOUGH:
    //reallocate ht_id
    int newTabaleSize = d->curArrSize * EXPANDING_FACTOR;
    UserInfoPtr *id_temp = (UserInfoPtr *)realloc(d->ht_id, newTableSize * sizeof(UserInfoPtr));
    if (id_temp == NULL)
    {
      return (-1);
    }
    else
    {
      d->ht_id = id_temp;
      id_temp = NULL;
      d->HtState = NORMAL;
      d->curArrSize = newTabaleSize;
    }
    //traverse ht_id
    UserInfoPtr ptr = NULL;
    for (int i = 0; i < ((d->curArrSize) / EXPANDING_FACTOR); i++)
    {
      ptr = d->ht_id[i];
      if (ptr == NULL)
      {
        continue;
      }
      //recalculate new hashId of the head of linked list
      int newHashId = hash_function(ptr->id, d->curArrSize);

      //link that head to toe d->ht_id[new hashId] & d->ht_name[new hashName]
      d->ht_id[i] = NULL;
      d->ht_id[newHashId] = ptr;
    }
    break;

  case NORMAL:
    return 0;
    break;
  }
}

static void resizingHtName(DB_T d)
{
}

/*
(Extra credit: 15%) Implement hash table expansion. 
When the number of nodes (user items) in a hash table reaches 75% of the number of buckets, expand the hash table to double the number of buckets. 
That is, your initial number of buckets is 1024. 
When the number of nodes reaches (0.75 * 1024), you expand the number of buckets to 2048. 
Again, when the number of nodes reaches (0.75 * 2048), the next number of buckets should be 4086. 
The max number of buckets is 1 million (1,048,576 = 2^20), 
so even if the number of nodes exceeds 1 million, don't expand the hash table. 
Note that even after hash table expansion, you should be able to retrieve all existing user items already registered before hash table expansion. 
Mark if you implement hash table expansion in your readme file.
*/
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
  d->HtState = NORMAL;

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
/*
On success, it should return 0. Otherwise, it should return -1.
If d or id is NULL, it is a failure. If no such item exists, it is a failure.
Make sure that you free all the memory allocated for the item being unregistered.
*/
int UnregisterCustomerByID(DB_T d, const char *id)
{
  // validate the paramter
  if (d == NULL || id == NULL)
  {
    return (-1);
  }

  // find the userPtr
  struct userChain userChainId = findUserById(d, id);

  // if the user doesn't exist, findUserById return ptr with NULL
  UserInfoPtr userPtr = userChainId.ptr;
  if (userPtr == NULL)
  {
    return (-1);
  }

  // if the user exists, copy the hashId and hashName
  int hashId = userChainId.hashId;
  int hashName = userChainId.hashName;

  //if that user is the first element in that linked list
  if (userChainId.ptr == d->ht_id[hashId])
  {
    struct UserInfo **head = &d->ht_id[hashId];
    *head = userPtr->nextId;
  }
  else
  {
    userChainId.prevPtr->nextId = userChainId.nextPtr;
  }
  //printf("%p\n", d->ht_id[hashId]);
  struct userChain userChainName = findUserByName(d, userPtr->name);
  if (userChainName.ptr == d->ht_name[hashName])
  {
    struct UserInfo **head = &d->ht_name[hashName];
    *head = userPtr->nextName;
  }
  else
  {
    userChainName.prevPtr->nextName = userChainName.nextPtr;
  }
  //printf("%p\n", d->ht_name[hashName]);

  userPtr->nextId = NULL;
  userPtr->nextName = NULL;
  free(userPtr->name);
  free(userPtr->id);
  free(userPtr);

  d->numItems--;

  return 0;
  /*search UserInfo in ht_id
    struct userChain userIdChain= findById(d,id)
    UserInfoPtr userPtr = UserChain.ptr;

    if(UserChain.ptr == d->ht_id[hashId]){
      UserInfo ** head = & d->ht_id[hashId];
      *head = ptr->nextId;
    }
    else{
      UserChain.prevPtr -> nextId = UserChain.nextPtr;
    }

    struct userChain userNameChain= findByName(d,userPtr->name)
    if(UserChain.ptr == d->ht_name[hashName]){
      UserInfo ** head = &d->ht_id[hashName];
      *head = ptr->nextId;
    }
    else{
      UserChain.prevPtr -> nextName = UserChain.nextPtr;
    }

    free(userPtr->name);
    free(userPtr->id);
    free(userPtr);

  */
}

/*--------------------------------------------------------------------*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
  // validate the paramter
  if (d == NULL || name == NULL)
  {
    return (-1);
  }

  // find the userPtr
  struct userChain userChainName = findUserByName(d, name);

  // if the user doesn't exist, findUserById return ptr with NULL
  UserInfoPtr userPtr = userChainName.ptr;
  if (userPtr == NULL)
  {
    return (-1);
  }

  // if the user exists, copy the hashId and hashName
  int hashId = userChainName.hashId;
  int hashName = userChainName.hashName;

  //if that user is the first element in that linked list
  if (userChainName.ptr == d->ht_name[hashName])
  {
    struct UserInfo **head = &d->ht_name[hashName];
    *head = userPtr->nextName;
  }
  else
  {
    userChainName.prevPtr->nextName = userChainName.nextPtr;
  }
  //printf("%p\n", d->ht_id[hashId]);
  struct userChain userChainId = findUserById(d, userPtr->id);
  if (userChainId.ptr == d->ht_id[hashId])
  {
    struct UserInfo **head = &d->ht_id[hashId];
    *head = userPtr->nextId;
  }
  else
  {
    userChainId.prevPtr->nextId = userChainId.nextPtr;
  }
  //printf("%p\n", d->ht_id[hashId]);

  userPtr->nextId = NULL;
  userPtr->nextName = NULL;
  free(userPtr->name);
  free(userPtr->id);
  free(userPtr);

  d->numItems--;

  return 0;
}
/*--------------------------------------------------------------------*/
int GetPurchaseByID(DB_T d, const char *id)
{
  //validate paramter
  if (d == NULL || id == NULL)
  {
    return (-1);
  }

  UserInfoPtr userPtr = findUserById(d, id).ptr;
  if (userPtr == NULL)
  {
    return (-1);
  }
  else
  {
    return userPtr->purchase;
  }
}
/*--------------------------------------------------------------------*/
int GetPurchaseByName(DB_T d, const char *name)
{
  //validate paramter
  if (d == NULL || name == NULL)
  {
    return (-1);
  }
  UserInfoPtr userPtr = findUserByName(d, name).ptr;
  if (userPtr == NULL)
  {
    return (-1);
  }
  else
  {
    return userPtr->purchase;
  }
}
/*--------------------------------------------------------------------*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  //validate paramter
  if (d == NULL || fp == NULL)
  {
    return (-1);
  }
  UserInfoPtr ptr = NULL;
  UserInfoPtr nextPtr = NULL;
  int sum = 0;
  for (int i = 0; i < d->curArrSize; i++)
  {
    for (ptr = d->ht_id[i]; ptr != NULL; ptr = nextPtr)
    {
      sum += (*fp)(ptr->id, ptr->name, ptr->purchase);
      nextPtr = ptr->nextId;
    }
  }
  return sum;
}