#include "ProcData.h"

void generate_random_data(StatData *data, size_t num_entries) {
    for (size_t i = 0; i < num_entries; i++) {
        data[i].id = rand() % 10000;
        data[i].count = rand() % 1000;
        data[i].cost = (float)rand() / 10;
        data[i].primary = rand() % 2;
        data[i].mode = rand() % 11;
    }
}

void InsertData()
{
     StatData *fist_array;
     StatData *sec_array;

     srand(time(NULL));

    fist_array = malloc(NUM_ENTRIES * sizeof(StatData));
    sec_array = malloc(NUM_ENTRIES * sizeof(StatData));

    generate_random_data(fist_array, NUM_ENTRIES);
    generate_random_data(sec_array, NUM_ENTRIES);

    if ((StoreDump(fist_array, NUM_ENTRIES, "file1") == FAILURE) ||
        (StoreDump(sec_array, NUM_ENTRIES, "file2") == FAILURE))
    {
        free(fist_array);
        free(sec_array);

        return;
    }

    free(fist_array);
    free(sec_array);

    return;
}
