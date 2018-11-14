/*
 Student Name: Zheyuan Xu
 Date:         11/7/2018

=======================
ECE 2035 Project 2-1:
=======================
This file provides definition for the structs and functions declared in the
header file. It also contains helper functions that are not accessible from
outside of the file.

FOR FULL CREDIT, BE SURE TO TRY MULTIPLE TEST CASES and DOCUMENT YOUR CODE.

===================================
Naming conventions in this file:
===================================
1. All struct names use camel case where the first letter is capitalized.
  e.g. "HashTable", or "HashTableEntry"

2. Variable names with a preceding underscore "_" will not be called directly.
  e.g. "_HashTable", "_HashTableEntry"

  Recall that in C, we have to type "struct" together with the name of the struct
  in order to initialize a new variable. To avoid this, in hash_table.h
  we use typedef to provide new "nicknames" for "struct _HashTable" and
  "struct _HashTableEntry". As a result, we can create new struct variables
  by just using:
    - "HashTable myNewTable;"
     or
    - "HashTableEntry myNewHashTableEntry;"

  The preceding underscore "_" simply provides a distinction between the names
  of the actual struct defition and the "nicknames" that we use to initialize
  new structs.
  [See Hidden Definitions section for more information.]

3. Functions, their local variables and arguments are named with camel case, where
  the first letter is lower-case.
  e.g. "createHashTable" is a function. One of its arguments is "numBuckets".
       It also has a local var
       iable called "newTable".

4. The name of a struct member is divided by using underscores "_". This serves
  as a distinction between function local variables and struct members.
  e.g. "num_buckets" is a member of "HashTable".

*/

/****************************************************************************
* Include the Public Interface
*
* By including the public interface at the top of the file, the compiler can
* enforce that the function declarations in the the header are not in
* conflict with the definitions in the file. This is not a guarantee of
* correctness, but it is better than nothing!
***************************************************************************/
#include "hash_table.h"


/****************************************************************************
* Include other private dependencies
*
* These other modules are used in the implementation of the hash table module,
* but are not required by users of the hash table.
***************************************************************************/
#include <stdlib.h>   // For malloc and free
#include <stdio.h>    // For printf


/****************************************************************************
* Hidden Definitions
*
* These definitions are not available outside of this file. However, because
* the are forward declared in hash_table.h, the type names are
* available everywhere and user code can hold pointers to these structs.
***************************************************************************/
/**
 * This structure represents an a hash table.
 * Use "HashTable" instead when you are creating a new variable. [See top comments]
 */
struct _HashTable {
  /** The array of pointers to the head of a singly linked list, whose nodes
      are HashTableEntry objects */
  HashTableEntry** buckets;

  /** The hash function pointer */
  HashFunction hash;

  /** The number of buckets in the hash table */
  unsigned int num_buckets;
};

/**
 * This structure represents a hash table entry.
 * Use "HashTableEntry" instead when you are creating a new variable. [See top comments]
 */
struct _HashTableEntry {
  /** The key for the hash table entry */
  unsigned int key;

  /** The value associated with this hash table entry */
  void* value;

  /**
  * A pointer pointing to the next hash table entry
  * NULL means there is no next entry (i.e. this is the tail)
  */
  HashTableEntry* next;
};


/****************************************************************************
* Private Functions
*
* These functions are not available outside of this file, since they are not
* declared in hash_table.h.
***************************************************************************/
/**
* createHashTableEntry
*
* Helper function that creates a hash table entry by allocating memory for it on
* the heap. It initializes the entry with key and value, initialize pointer to
* the next entry as NULL, and return the pointer to this hash table entry.
*
* @param key The key corresponds to the hash table entry
* @param value The value stored in the hash table entry
* @return The pointer to the hash table entry
*/
static HashTableEntry* createHashTableEntry(unsigned int key, void* value) {
  HashTableEntry* newEntry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
  newEntry->key = key;
  newEntry->value = value;
  newEntry->next = NULL;
  return newEntry;
}

/**
* findItem
*
* Helper function that checks whether there exists the hash table entry that
* contains a specific key.
*
* @param hashTable The pointer to the hash table.
* @param key The key corresponds to the hash table entry
* @return The pointer to the hash table entry, or NULL if key does not exist
*/
static HashTableEntry* findItem(HashTable* hashTable, unsigned int key) {
  //get the hash index
  unsigned int index = hashTable->hash(key);
  //point to the starting index of the bucket
  HashTableEntry* entry = hashTable->buckets[index];
  //while entry is not null, i.e. not the end of the bucket
  while (entry) {
    //if key matches return that entry
    if (entry->key == key) {
      return entry;
    }
    //otherwise move to the next entry in that bucket
    entry = entry->next;
  }
  return entry; //return entry found, if not found it will point to the null entry pointed by the last entry in that bucket
}


/****************************************************************************
* Public Interface Functions
*
* These functions implement the public interface as specified in the header
* file, and make use of the private functions and hidden definitions in the
* above sections.
****************************************************************************/
// The createHashTable is provided for you as a starting point.
HashTable* createHashTable(HashFunction hashFunction, unsigned int numBuckets) {
  // The hash table has to contain at least one bucket. Exit gracefully if
  // this condition is not met.
  if (numBuckets==0) {
    printf("Hash table has to contain at least 1 bucket...\n");
    exit(1);
  }

  // Allocate memory for the new HashTable struct on heap.
  HashTable* newTable = (HashTable*)malloc(sizeof(HashTable));

  // Initialize the components of the new HashTable struct.
  newTable->hash = hashFunction;
  newTable->num_buckets = numBuckets;
  newTable->buckets = (HashTableEntry**)malloc(numBuckets*sizeof(HashTableEntry*));

  // As the new buckets contain indeterminant values, init each bucket as NULL.
  unsigned int i;
  for (i=0; i<numBuckets; ++i) {
    newTable->buckets[i] = NULL;
  }

  // Return the new HashTable struct.
  return newTable;
}

void destroyHashTable(HashTable* hashTable) {
  //iterate from 0 to the end of buckets
  for (int i = 0; i < (hashTable->num_buckets); i++) {
    if (hashTable->buckets[i]) {
      //create a pointer pointing to the start of the bucket and another pointing to the next entry
      HashTableEntry* entry = hashTable->buckets[i];
      HashTableEntry* nextEntry = entry->next;
      //while nextEntry is not null
      while (nextEntry) {
        //free all the data stored in entry
        free(entry->value);
        free(entry);
        //move current entry to the next
        entry = nextEntry;
        //move next entry to next of next entry
        nextEntry = nextEntry->next;
      }
      //free current entry
      free(entry);
    }
  }
  //free the hashTable after it has emptied its buckets
  free(hashTable);
  return;
}

void* insertItem(HashTable* hashTable, unsigned int key, void* value) {
  //get the index to insert
   int index = hashTable->hash(key);
   //If there exists entry with matching key
   HashTableEntry* entry = findItem(hashTable,key);
   //if entry with same key is found, update the value
   if (entry) {
     //store the copy of old value
     void* oldValue;
     oldValue = entry -> value;
     entry-> value = value;
     return oldValue;
   }
   //create new entry
   HashTableEntry* newEntry = createHashTableEntry(key, value);
   //if newEntry is null
   if(!newEntry) {
     return NULL;
   }
   //insert new entry to the front
   newEntry->next = hashTable->buckets[index];
   hashTable->buckets[index] = newEntry;
   return NULL;
}

void* getItem(HashTable* hashTable, unsigned int key) {
  HashTableEntry* entry = findItem(hashTable,key);
  //if entry with same key is found, i.e. entry is not null
  if(entry)
  {
    return entry->value;
  }
  //else return null
  return NULL;
}

void* removeItem(HashTable* hashTable, unsigned int key) {
  //get hash index
  int index = hashTable -> hash(key);
  //create a pointer pointing to the head of bucket
  HashTableEntry* entry = hashTable -> buckets[index];

  //if head has the same key, remove head
  if(entry && entry->key == key)
  {
    //get value of entry to be removed
    void* toRemove = entry->value;
    // update head
    hashTable->buckets[index] = entry->next;
    //remove head entry
    free(entry);
    return toRemove;
  }

  //else, if key is not matching, keep probing the bucket
  while(entry && entry -> next)
  {
    //check if next entry is one to remove
    if(entry->next->key == key)
    {
      //save entry to be removed
      HashTableEntry* toRemove = entry -> next;
      //save value removed entry
      void* removedEntry = toRemove -> value;
      //remove the entry
      entry-> next = entry->next->next;
      free(toRemove);
      //return value of removed entry
      return removedEntry;
    }
    //keep probing
    entry = entry->next;
  }
  //entry not found, return null
  return NULL;
}

void deleteItem(HashTable* hashTable, unsigned int key) {
  //get bucket index
  int index = hashTable->hash(key);
  //get bucket pointer
  HashTableEntry* entry = hashTable->buckets[index];
  //if not found end the function
  if (findItem(hashTable,key) == NULL) {
    return;
  }
  //if found and the starting entry matches the key, remove starting entry
  if (entry && entry->key == key)
  {
    //set starting entry equal to next entry in the bucket
    hashTable->buckets[index] = entry->next;
    //free starting entry value
    free(entry -> value);
    //free starting entry
    free(entry);
    //end the function
    return;
  }

  //delete if starting entry is not the entry to be deleted
  while(entry && entry-> next)
  {
    //if next entry is the one to be deleted
    if(entry -> next -> key == key)
    {
      //save pointer to the deleted entry
      HashTableEntry* toDelete = entry-> next;
      //remove entry
      entry->next = entry->next->next;
      //free value of removed entry
      free(toDelete -> value);
      //free entry
      free(toDelete);
      //end the function
      return;
    }
    //keep probing the bucket
    entry = entry -> next;
  }
}
