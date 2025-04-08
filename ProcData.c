#include "ProcData.h"

char buf[4];

unsigned int hash_long(long key)
{
    return (unsigned int)(key ^ (key >> 32)) % HASH_SIZE;
}

void ProcEntry(HashEntry **hash_table, const StatData *entry)
{
    HashEntry **pp;
    unsigned int idx = 0;

    idx = hash_long(entry->id);
    pp = &hash_table[idx];

    while (*pp)
    {
        if ((*pp)->key == entry->id)
        {
            (*pp)->value.count += entry->count;
            (*pp)->value.cost += entry->cost;
            (*pp)->value.primary &= entry->primary;

            if (entry->mode > (*pp)->value.mode)
            {
                (*pp)->value.mode = entry->mode;
            }

            return;
        }

        pp = &(*pp)->next;
    }

    HashEntry *new_entry = malloc(sizeof(HashEntry));

    if (new_entry == NULL)
    {
        return;
    }

    new_entry->key = entry->id;
    new_entry->value = *entry;
    new_entry->next = NULL;
    *pp = new_entry;
}

int StoreDump(const StatData *data, size_t count, const char *filename)
{
    FILE *file;
    size_t wr;

    file = fopen(filename, "wb");

    if (!file)
    {
        return FAILURE;
    }

    wr = fwrite(data, sizeof(StatData), count, file);

    if (wr != count)
    {
        fclose(file);
        return FAILURE;
    }

    fclose(file);

    return SUCCESS;
}

StatData *LoadDump(const char *filename, size_t *count_out)
{
    StatData *data;
    FILE *file;
    size_t read = 0;
    size_t count = NUM_ENTRIES;

    file = fopen(filename, "rb");

    if (file == NULL)
    {
        printf ("1test\n");
        return NULL;
    }

    data = malloc(count * sizeof(StatData));

    if (data == NULL)
    {
        fclose(file);
        return NULL;
    }

    read = fread(data, sizeof(StatData), count, file);

    if (read < 1)
    {
        printf ("3test\n");
        fclose(file);
        return NULL;
    }

    fclose(file);
    *count_out = count;

    return data;
}

StatData *JoinDump(const StatData *a, size_t a_count, const StatData *b,
                   size_t b_count, size_t *out_count)
{

    HashEntry **hash_table;
    StatData *result;
    size_t total = 0;
    size_t pos = 0;

    hash_table = calloc(HASH_SIZE, sizeof(HashEntry *));

    if (hash_table == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < a_count; i++)
    {
        ProcEntry(hash_table, &a[i]);
    }
    for (size_t i = 0; i < b_count; i++)
    {
        ProcEntry(hash_table, &b[i]);
    }

    for (size_t i = 0; i < HASH_SIZE; i++)
    {
        HashEntry *entry = hash_table[i];
        while (entry)
        {
            total++;
            entry = entry->next;
        }
    }

    result = malloc(total * sizeof(StatData));

    if (!result)
    {
        for (size_t i = 0; i < HASH_SIZE; i++)
        {
            HashEntry *entry = hash_table[i];
            while (entry)
            {
                HashEntry *next = entry->next;
                free(entry);
                entry = next;
            }
        }
        free(hash_table);
        return NULL;
    }

    for (size_t i = 0; i < HASH_SIZE; i++)
    {
        HashEntry *entry = hash_table[i];
        while (entry)
        {
            result[pos++] = entry->value;
            HashEntry *next = entry->next;
            free(entry);
            entry = next;
        }
    }

    free(hash_table);
    *out_count = total;

    return result;
}

static int compare_cost(const void *a, const void *b)
{
    float diff = ((const StatData *)a)->cost - ((const StatData *)b)->cost;
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

void SortDump(StatData *data, size_t count)
{
    qsort(data, count, sizeof(StatData), compare_cost);
}

const char *mode_to_bin(unsigned int mode)
{
    for (int i = 2; i >= 0; i--)
    {
        if (mode & 1)
        {
            buf[i] = '1';
        }
        else
        {
            buf[i] = '0';
        }

        mode >>= 1;
    }

    buf[3] = '\0';

    return buf;
}

void print_table(const StatData *data, size_t count, FILE *stream)
{
    fprintf(stream, "%-15s | %-10s | %-15s | %-7s | %-5s\n",
            "ID", "Count", "Cost", "Primary", "Mode");
    fprintf(stream, "------------------------------------------------\n");

    for (size_t i = 0; i < count; i++)
    {
        fprintf(stream, "%ld | %-8d | %-15.3e | %-7s | %s\n",
                data[i].id,
                data[i].count,
                data[i].cost,
                data[i].primary ? "y" : "n",
                mode_to_bin(data[i].mode));
    }
}

int main(int argc, char **argv)
{
    StatData *JoinResult;
    StatData *data_a;
    StatData *data_b;
    size_t total_count;
    size_t count_a;
    size_t count_b;
    size_t result_test_fail = 0;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> <output>\n", argv[0]);

        return FAILURE;
    }

    result_test_fail += test_process_entry();
    result_test_fail += test_store_load();
    result_test_fail += test_join();
    result_test_fail += test_sort();

    printf("\n\033[1mTOTAL TESTS FAILED: %ld\033[0m\n", result_test_fail);

    if (result_test_fail > 0)
    {
        return FAILURE;
    }

    InsertData();

    data_a = LoadDump(argv[1], &count_a);
    data_b = LoadDump(argv[2], &count_b);

    if (data_a == NULL || data_b == NULL)
    {
        fprintf(stderr, "Error loading files\n");
        free(data_a);
        free(data_b);

        return FAILURE;
    }

    JoinResult = JoinDump(data_a, count_a, data_b, count_b, &total_count);

    free(data_a);
    free(data_b);

    if (JoinResult == NULL)
    {
        fprintf(stderr, "Error joining data\n");

        return FAILURE;
    }

    SortDump(JoinResult, total_count);

    print_table(JoinResult, total_count, stdout);

    if (StoreDump(JoinResult, total_count, argv[3]) != 0)
    {
        fprintf(stderr, "Error saving result\n");
        free(JoinResult);

        return FAILURE;
    }

    free(JoinResult);

    return SUCCESS;
}