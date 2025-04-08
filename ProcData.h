#ifndef PROCDATA_H
#define PROCDATA_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define SUCCESS 0
#define FAILURE 1

#define TRUE 1
#define FALSE 0

#define NUM_ENTRIES 10

#define HASH_SIZE 1000037

typedef struct Data
{
    long id;
    int count;
    float cost;
    unsigned int primary : 1;
    unsigned int mode : 3;
} StatData;

typedef struct HashEntry
{
    long key;
    StatData value;
    struct HashEntry *next;
} HashEntry;

int StoreDump(const StatData *, size_t, const char *);
StatData *LoadDump(const char *, size_t *);
StatData *JoinDump(const StatData *, size_t, const StatData *, size_t, size_t *);
void SortDump(StatData *, size_t);
unsigned int hash_long(long);
void ProcEntry(HashEntry **, const StatData *);
void generate_random_data(StatData *, size_t);
void InsertData();
int test_process_entry();
int test_store_load();
int test_join();
int test_sort();

#endif