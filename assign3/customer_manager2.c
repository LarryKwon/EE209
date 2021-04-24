#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define BUCKET_COUNT 20
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
  int curHtSize;
  enum HtState HtState;
};

struct userChain
{
  UserInfoPtr ptr;
  UserInfoPtr prevPtr;
  UserInfoPtr nextPtr;
};

static struct userChain findUserById(DB_T d, const char *id)
{
  struct userChain userChainId;
  userChainId.ptr = NULL;
  userChainId.prevPtr = NULL;
  userChainId.nextPtr = NULL;

  UserInfoPtr current = NULL;
  UserInfoPtr next = NULL;
  UserInfoPtr prev = NULL;
  for (int i = 0; i < d->curHtSize; i++)
  {
    for (current = d->ht_id[i]; current != NULL; current = next)
    {
      next = current->nextId;
      if (strcmp(current->id, id) == 0)
      {
        userChainId.ptr = current;
        userChainId.prevPtr = prev;
        userChainId.nextPtr = next;
        return userChainId;
      }
      prev = current;
    }
  }

  return userChainId;
}

/*------------------------------------------------------------------*/
static struct userChain findUserByName(DB_T d, const char *name)
{
  struct userChain userChainName;
  userChainName.ptr = NULL;
  userChainName.prevPtr = NULL;
  userChainName.nextPtr = NULL;

  UserInfoPtr current = NULL;
  UserInfoPtr next = NULL;
  UserInfoPtr prev = NULL;
  for (int i = 0; i < d->curHtSize; i++)
  {
    for (current = d->ht_name[i]; current != NULL; current = next)
    {
      next = current->nextName;
      if (strcmp(current->name, name) == 0)
      {
        userChainName.ptr = current;
        userChainName.prevPtr = prev;
        userChainName.nextPtr = next;
        return userChainName;
      }
      prev = current;
    }
  }

  return userChainName;
}
/*------------------------------------------------------------------*/
static void assertHtSize(DB_T d)
{
  assert(d);
  if (d->numItems >= d->curHtSize * EXPANDING_CRITERIA)
  {
    d->HtState = ENOUGH;
  }
  else
  {
    d->HtState = NORMAL;
  }
}

/*------------------------------------------------------------------*/
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
static int ResizingHt(DB_T d)
{

  assertHtSize(d);
  int curTableSize = d->curHtSize;
  int curNumItems = d->numItems;
  UserInfoPtr *ht_id_temp = d->ht_id;
  UserInfoPtr *ht_name_temp = d->ht_name;

  int newTableSize;
  UserInfoPtr *newHtId = NULL;
  UserInfoPtr *newHtName = NULL;
  switch (d->HtState)
  {
  case ENOUGH:
    //reallocate ht_id
    newTableSize = d->curHtSize * EXPANDING_FACTOR;
    newHtId = (UserInfoPtr *)calloc(newTableSize, sizeof(UserInfoPtr));
    newHtName = (UserInfoPtr *)calloc(newTableSize, sizeof(UserInfoPtr));
    if (newHtId == NULL)
    {
      return (-1);
    }
    else if (newHtName == NULL)
    {
      return (-1);
    }
    else
    {
      d->ht_id = newHtId;
      newHtId = NULL;
      d->ht_name = newHtName;
      newHtName = NULL;
      d->HtState = NORMAL;
      d->curHtSize = newTableSize;
    }

    //traverse ht_id_temp and unregister the user and register to the new d;
    UserInfoPtr temp = NULL;
    UserInfoPtr ptr = NULL;
    UserInfoPtr nextPtr = NULL;
    for (int i = 0; i < curTableSize; i++)
    {
      for (ptr = ht_id_temp[i]; ptr != NULL; ptr = nextPtr)
      {
        nextPtr = ptr->nextId;

        //copy items
        char *id_copy = NULL;
        char *name_copy = NULL;
        if ((id_copy = strdup(ptr->id)) == NULL)
        {
          return (-1);
        }
        if ((name_copy = strdup(ptr->name)) == NULL)
        {
          return (-1);
        }
        int purchase_copy = ptr->purchase;
        //int hashName_temp = hash_function(name_copy, d->curHtSize / EXPANDING_FACTOR);

        //Register to the new d;
        if (RegisterCustomer(d, id_copy, name_copy, purchase_copy) == (-1))
        {
          return (-1);
        }
        free(id_copy);
        free(name_copy);

        //free original
        free(ptr->id);
        free(ptr->name);
        ptr->id = NULL;
        ptr->name = NULL;
        ptr->purchase = 0;

        ptr->nextId = NULL;
        ptr->nextName = NULL;

        free(ptr);
      }
      ht_id_temp[i] = NULL;
      ht_name_temp[i] = NULL;
    }
    free(ht_id_temp);
    free(ht_name_temp);
    break;

  case NORMAL:
    return 0;
    break;
  }
}

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
  d->curHtSize = BUCKET_COUNT;
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
    for (int i = 0; i < d->curHtSize; i++)
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
      d->ht_id[i] = NULL;
      d->ht_name[i] = NULL;
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

  if (ResizingHt(d) == (-1))
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
  int hashName = hash_function(newUser->name, d->curHtSize);
  int hashId = hash_function(newUser->id, d->curHtSize);
  //printf("hashName: %d , hashId: %d\n", hashName, hashId);

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
    //printf("%p\n", d->ht_name[hashName]);
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
  UserInfoPtr user = userChainId.ptr;
  // if the user doesn't exist, findUserById return ptr with NULL
  if (user == NULL)
  {
    return (-1);
  }

  // if the user exists, copy the hashId and hashName
  int hashId = hash_function(user->id, d->curHtSize);
  int hashName = hash_function(user->name, d->curHtSize);

  //if that user is the first element in that linked list
  if (user == d->ht_id[hashId])
  {
    struct UserInfo **head = &d->ht_id[hashId];
    *head = user->nextId;
  }
  else
  {
    userChainId.prevPtr->nextId = userChainId.nextPtr;
  }
  //printf("%p\n", d->ht_id[hashId]);
  struct userChain userChainName = findUserByName(d, user->name);
  if (user == d->ht_name[hashName])
  {
    struct UserInfo **head = &d->ht_name[hashName];
    *head = user->nextName;
  }
  else
  {
    userChainName.prevPtr->nextName = userChainName.nextPtr;
  }
  //printf("%p\n", d->ht_name[hashName]);

  user->nextId = NULL;
  user->nextName = NULL;
  free(user->name);
  free(user->id);
  free(user);

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
  UserInfoPtr user = userChainName.ptr;
  // if the user doesn't exist, findUserById return ptr with NULL
  if (user == NULL)
  {
    return (-1);
  }

  // if the user exists, copy the hashId and hashName
  int hashId = hash_function(user->id, d->curHtSize);
  int hashName = hash_function(user->name, d->curHtSize);

  //if that user is the first element in that linked list
  if (user == d->ht_name[hashName])
  {
    struct UserInfo **head = &d->ht_name[hashName];
    *head = user->nextName;
  }
  else
  {
    userChainName.prevPtr->nextName = userChainName.nextPtr;
  }
  //printf("%p\n", d->ht_id[hashId]);
  struct userChain userChainId = findUserById(d, user->id);
  if (user == d->ht_id[hashId])
  {
    struct UserInfo **head = &d->ht_id[hashId];
    *head = user->nextId;
  }
  else
  {
    userChainId.prevPtr->nextId = userChainId.nextPtr;
  }
  //printf("%p\n", d->ht_name[hashName]);

  user->nextId = NULL;
  user->nextName = NULL;
  free(user->name);
  free(user->id);
  free(user);

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

  UserInfoPtr user = findUserById(d, id).ptr;
  if (user == NULL)
  {
    return (-1);
  }
  return user->purchase;
}
/*--------------------------------------------------------------------*/
int GetPurchaseByName(DB_T d, const char *name)
{
  //validate paramter
  if (d == NULL || name == NULL)
  {
    return (-1);
  }

  UserInfoPtr user = findUserByName(d, name).ptr;
  if (user == NULL)
  {
    return (-1);
  }
  return user->purchase;
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
  for (int i = 0; i < d->curHtSize; i++)
  {
    for (ptr = d->ht_id[i]; ptr != NULL; ptr = nextPtr)
    {
      sum += (*fp)(ptr->id, ptr->name, ptr->purchase);
      nextPtr = ptr->nextId;
    }
  }
  return sum;
}