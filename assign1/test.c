// 20180036 권혁태 customer_manager1.c//
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
// create DB//
/*
parameter: void
return DB_T
*/
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
    d->pArray = (struct UserInfo *)calloc(d->curArrSize,
                                          sizeof(struct UserInfo));
    if (d->pArray == NULL)
    {
        fprintf(stderr, "Can't allocate a memory for array of size %d\n",
                d->curArrSize);
        free(d);
        return NULL;
    }
    return d;

    return NULL;
}
//destroy DB//
/*
free all DB's item
parameter: DB_T 
return: void
*/
void DestroyCustomerDB(DB_T d)
{
    /* fill out this function */
    int i;
    for (i = 0; i < d->curArrSize; i++)
    {
        free(d->pArray[i].id);
        free(d->pArray[i].name);
    }
    free(d->pArray);
    free(d);
}
/*--------------------------------------------------------------------*/
//register item//
/*
add items into the array
parameter: DB_T, info of USERS(id, name, purchase)
return: int
if success -> 0
else -> -1
*/
int RegisterCustomer(DB_T d, const char *id,
                     const char *name, const int purchase)
{
    /* fill out this function */
    //check the d,id,name,purchase is null//
    assert(d != NULL);
    assert(id != NULL);
    assert(name != NULL);
    assert(purchase >= 0);

    //copy all the items//
    char *new_id = NULL;
    char *new_name = NULL;
    new_id = strdup(id);
    new_name = strdup(name);
    //check for the array size//
    if (d->numItems == d->curArrSize)
    {
        d->curArrSize = d->curArrSize + 1024;
        d->pArray = (struct UserInfo *)realloc(
            d->pArray, d->curArrSize * sizeof(struct UserInfo));
        if (d->pArray == NULL)
        {
            return (-1);
        }
    }
    //check if there is same thing//
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            if (strcmp(d->pArray[i].name, new_name) == 0)
            {
                return (-1);
            }
            if (strcmp(d->pArray[i].id, new_id) == 0)
            {
                return (-1);
            }
        }
    }

    //insert into empty element//
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name == NULL)
        {
            d->pArray[i].id = new_id;
            d->pArray[i].name = new_name;
            d->pArray[i].purchase = purchase;
            break;
        }
    }

    // free(new_name);
    // free(new_id);
    d->numItems++;

    return 0;
}
/*--------------------------------------------------------------------*/
//unregister item by id//
/*
delete items from the array
parameter: DB_T, id
return: int
if success -> 0
else -> -1
*/
int UnregisterCustomerByID(DB_T d, const char *id)
{
    /* fill out this function */
    assert(d != NULL);
    assert(id != NULL);
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            if (strcmp(d->pArray[i].id, id) == 0)
            {
                free(d->pArray[i].name);
                free(d->pArray[i].id);
                d->pArray[i].name = NULL;
                d->pArray[i].id = NULL;
                return 0;
                break;
            }
        }
    }
    return (-1);
}

/*--------------------------------------------------------------------*/
//unregister item by name//
/*
delete items from the array
parameter: DB_T, name
return: int
if success -> 0
else -> -1
*/
int UnregisterCustomerByName(DB_T d, const char *name)
{
    /* fill out this function */
    assert(d != NULL);
    assert(name != NULL);
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            if (strcmp(d->pArray[i].name, name) == 0)
            {
                free(d->pArray[i].name);
                free(d->pArray[i].id);
                d->pArray[i].name = NULL;
                d->pArray[i].id = NULL;
                return 0;
                break;
            }
        }
    }
    return (-1);
}
/*--------------------------------------------------------------------*/
//get purchase information by using id//
/*
search id in DB
when it finds, get the purchase info 
parameter: DB_T, id
return: int
if success -> User information(purchase);
else -> -1
*/
int GetPurchaseByID(DB_T d, const char *id)
{
    /* fill out this function */
    assert(d != NULL);
    assert(id != NULL);
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            if (strcmp(d->pArray[i].id, id) == 0)
            {
                return d->pArray[i].purchase;
            }
        }
    }
    return (-1);
}
/*--------------------------------------------------------------------*/
//get purchase information by using name//
/*
search name in DB
when it finds, get the purchase info 
parameter: DB_T, name
return: int
if success -> User information(purchase);
else -> -1
*/
int GetPurchaseByName(DB_T d, const char *name)
{
    /* fill out this function */
    assert(d != NULL);
    assert(name != NULL);
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            if (strcmp(d->pArray[i].name, name) == 0)
            {
                return d->pArray[i].purchase;
            }
        }
    }
    return (-1);
}
/*--------------------------------------------------------------------*/
//sum the purchase info of users who are matched by fp//
/*
search id in DB
when it finds, get the purchase info 
sum all the purchase info
parameter: DB_T, fp
fp -> function pointer
return: int
sum all the purchase info of users matched with fp
*/
int GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
    /* fill out this function */
    if (d == NULL)
    {
        return -1;
    }
    if (fp == NULL)
    {
        return -1;
    }
    int sum = 0;
    for (int i = 0; i < d->curArrSize; i++)
    {
        if (d->pArray[i].name != NULL)
        {
            sum = sum + fp(d->pArray[i].id, d->pArray[i].name,
                           d->pArray[i].purchase);
        }
    }
    return sum;
}