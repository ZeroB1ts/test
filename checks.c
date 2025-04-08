#include "checks.h"

StatData create_entry(long id, int count, float cost, int primary, int mode)
{
    return (StatData){id, count, cost, primary, mode};
}

void free_hash_table(HashEntry **table)
{
    for (size_t i = 0; i < HASH_SIZE; i++)
    {
        HashEntry *entry = table[i];
        while (entry)
        {
            HashEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(table);
}

int test_process_entry()
{
    TEST("Func ProcEntry");
    HashEntry **table = calloc(HASH_SIZE, sizeof(HashEntry *));

    StatData e1 = create_entry(123, 5, 10.0f, 1, 3);
    ProcEntry(table, &e1);

    unsigned int idx = hash_long(123);
    HashEntry *entry = table[idx];
    ASSERT(entry != NULL);
    ASSERT_EQUAL(entry->value.count, 5);
    ASSERT_FLOAT_EQUAL(entry->value.cost, 10.0f);

    StatData e2 = create_entry(123, 2, 5.0f, 0, 5);
    ProcEntry(table, &e2);

    ASSERT_EQUAL(entry->value.count, 7);
    ASSERT_FLOAT_EQUAL(entry->value.cost, 15.0f);
    ASSERT_EQUAL(entry->value.primary, 0);
    ASSERT_EQUAL(entry->value.mode, 5);

    free_hash_table(table);
    return SUCCESS;
}

int test_store_load()
{
    TEST("Funcs StoreDump/LoadDump");
    const char *filename = "test.bin";
    StatData data[] = {
        create_entry(1, 10, 100.5f, 1, 2),
        create_entry(2, 20, 200.3f, 0, 5)};
    const size_t count = sizeof(data) / sizeof(data[0]);

    ASSERT_EQUAL(StoreDump(data, count, filename), SUCCESS);

    size_t read_count;
    StatData *loaded = LoadDump(filename, &read_count);
    ASSERT(loaded != NULL);

    for (size_t i = 0; i < count; i++)
    {
        ASSERT_EQUAL(loaded[i].id, data[i].id);
        ASSERT_EQUAL(loaded[i].count, data[i].count);
        ASSERT_FLOAT_EQUAL(loaded[i].cost, data[i].cost);
    }

    free(loaded);
    remove(filename);

    return SUCCESS;
}

int test_join()
{
    TEST("Func JoinDump");
    StatData a[] = {create_entry(1, 2, 3.0f, 1, 2)};
    StatData b[] = {create_entry(1, 3, 4.0f, 0, 5)};

    size_t out_count;
    StatData *result = JoinDump(a, 1, b, 1, &out_count);

    ASSERT(result != NULL);
    ASSERT_EQUAL(out_count, 1);
    ASSERT_EQUAL(result[0].count, 5);
    ASSERT_FLOAT_EQUAL(result[0].cost, 7.0f);
    ASSERT_EQUAL(result[0].primary, 0);
    ASSERT_EQUAL(result[0].mode, 5);

    free(result);

    return SUCCESS;
}

int test_sort()
{
    TEST("Func SortDump");
    StatData data[] = {
        create_entry(3, 0, 5.0f, 0, 0),
        create_entry(1, 0, 3.0f, 0, 0),
        create_entry(2, 0, 7.0f, 0, 0)};

    SortDump(data, 3);

    ASSERT_FLOAT_EQUAL(data[0].cost, 3.0f);
    ASSERT_FLOAT_EQUAL(data[1].cost, 5.0f);
    ASSERT_FLOAT_EQUAL(data[2].cost, 7.0f);

    return SUCCESS;
}

/*     StatData fist_array[2] =
        {{.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
         {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0}};
    StatData sec_array[2] =
        {{.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2},
         {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2}}; */