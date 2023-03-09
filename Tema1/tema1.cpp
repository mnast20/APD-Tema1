// Nastase Maria, 331CA

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <string>

#include "Reducer.h"

using namespace std;

/**
 * Mapping function processing the files and adding the numbers
 * to the exponents lists
 * @param arg the mapper
 * */
void* map_function(void *arg)
{
	// convert the argument to a mapper
    mapper* map = (mapper*) arg;

	string filename;

	// iterate through files
    while (map->check_file_availability()) {
		// delimit critical section
        pthread_mutex_lock(map->mutex_map);
		// get next file
        string filename = map->get_file();

		// pop file from queue
		map->file_queue->pop();
		pthread_mutex_unlock(map->mutex_map);
        
		// process the file using binary search
        map->process_file_with_bin_search(filename);
    }

	// wait for all the mappers to finish
	pthread_barrier_wait(map->barrier_map);

	// mappers are done, meaning reducers can now start
	*(map->mapper_done) = 1;

	// exit thread
	pthread_exit(NULL);
}

/**
 * Reducing function that extracts and counts all the numbers in
 * the mappers' exponent lists
 * @param arg the reducer
 * */
void* reduce_function(void *arg) {
    reducer *reduce = (reducer*) arg;
    
	// block reducers until the mappers finish
	while (!*(reduce->start_reducer)) {}

	// extract all the numbers from the exponent lists
	reduce->process_mapper_lists();
	// count these numbers and print them in a file
	reduce->unique_elements();

	// free the reducer data
	delete reduce;

	// exit thread
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int nr_mappers, nr_reducers; // number of mappers and reducers
	string filename; // file name
	ifstream file;
	queue<string> file_queue; // queue of files
	int nr_files = 0; // number of files
	int mapper_done = 0; // variable signifying that the mappers are done

	// map barrier and mutex
	pthread_barrier_t barrier_map;
	pthread_mutex_t mutex_map;

	// vector of mappers
	vector<mapper*> mappers;

	// check the number of arguments
	if (argc != 4) {
		cout << "Incorrect number of arguments\n";
		return -1;
	}

	// get number of mappers, reducers and the file name from the command line
	nr_mappers = atoi(argv[1]);
	nr_reducers = atoi(argv[2]);
	filename = argv[3];

	// open the file
	file.open(filename);

	// get the number of files that will e handled by the mappers
	string line;
	if (std::getline(file, line)) {
		nr_files = stoi(line);
	} else {
		cout << "Empty file\n";
	}

	// add files to the file queue
	while(std::getline(file, line)) {
		file_queue.push(line);
	}

	// close file
	file.close();

	// calculate number of threads
	long long int nr_threads = nr_mappers + nr_reducers;

	// argument and function that will be used for the thread functions
	void *argument;
	void* (*function) (void *);

	int i, r;
	void *status;
	pthread_t threads[nr_threads];

	// create the barrier and the mutex
	pthread_barrier_init(&barrier_map, NULL, nr_mappers);
	pthread_mutex_init(&mutex_map, NULL);

	for (i = 0; i < nr_threads; i++) {
		if (i < nr_mappers) {
			// format the mapper thread
			function = map_function;

			// create the mapper
			mapper *map = new mapper(i, &file_queue, nr_reducers, nr_files,
									&barrier_map, &mutex_map, &mapper_done);

			// add mapper to the mapper list
			mappers.push_back(map);
			argument = mappers[mappers.size() - 1];
		} else {
			// format the reducer thread
			function = reduce_function;

			// create the reducer
			reducer *reduce = new reducer(i - nr_mappers, nr_mappers,
											&mappers, &mapper_done);

			argument = reduce;
		}

		// create the thread
		r = pthread_create(&threads[i], NULL, function, argument);

		// treat error case
		if (r) {
			printf("Error at the creation of the thread %d\n", i);
			exit(-1);
		}
	}

	// start threads
	for (i = 0; i < nr_threads; i++) {
		r = pthread_join(threads[i], &status);

		// treat error case
		if (r) {
			printf("Error at waiting for the thread %d\n", i);
			exit(-1);
		}
	}

	// free the memory allocated to the mappers
	for (int i = 0; i < nr_mappers; i++) {
		delete mappers[i];
	}

	// destroy mutex and barrier
	pthread_mutex_destroy(&mutex_map);
	pthread_barrier_destroy(&barrier_map);

	return 0;
}