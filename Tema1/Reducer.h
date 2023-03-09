// Nastase Maria, 331CA

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <string>
#include <algorithm>
#include <set>
#include <pthread.h>

#include "Mapper.h"

using namespace std;

struct reducer {
    int id; // reducer id
    int nr_mappers; // number of mappers
    vector<mapper*> *mappers; // mappers vector
    int *start_reducer; // variable used to activate reducers
    set<long long int> exponent_list; // exponent list
    long long int nr_unique_values; // number of unique values

    reducer() {}

    /**
    * Reducer constructor
    * @param index reducer id
    * @param mappers_nr number of mappers
    * @param mappers_list list of mappers
    * @param mapper_done pointer signifying that the mappers are done
    * */
    reducer(int index, int mappers_nr, vector<mapper*> *mappers_list,
                    int *mapper_done) {
        id = index;
        nr_mappers = mappers_nr;
        nr_unique_values = 0;
        mappers = mappers_list;
        start_reducer = mapper_done; 
    }

    /**
    * Function processing the list of mappers and extracting the lists
    * matching the reducer's exponent
    * */
    void process_mapper_lists() {
        // iterate through mappers
        for (int i = 0; i < nr_mappers; ++i) {
            // insert the set corresponding to the exponent in
            // the reducer's list
            exponent_list.insert((*mappers)[i]->exponent_lists[id].begin(),
                                (*mappers)[i]->exponent_lists[id].end());
        }
    }

    void unique_elements() {
        // get the number of unique values
        nr_unique_values = exponent_list.size();

        // create the output file
        string str = "out" + to_string(id + 2) + ".txt";
        std::fstream file(str, std::ios::out | std::ios::in | std::ios::trunc);

        // create the output file
        file << nr_unique_values;
    }
};