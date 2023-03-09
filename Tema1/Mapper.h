// Nastase Maria, 331CA

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <string>
#include <set>
#include <pthread.h>
#include <algorithm>
#include <math.h>

using namespace std;

struct mapper {
	queue<string> *file_queue; // queue containing all files
    int id; // mapper index
    int max_exponent; // maximum exponent
    vector<set<long long int>> exponent_lists; // lists vector for each exponent
    int nr_files; // number of files in the file vector
    pthread_barrier_t *barrier_map; // barrier used to synchronize mappers
    pthread_mutex_t *mutex_map; // mutex delimiting a critical section
    int* mapper_done; // variable used to determine whether mappers are done

    mapper() {};

    /**
    * Mapper constructor
    * @param number mapper id
    * @param string_queue queue of file names
    * @param nr_reducers number of reducers to determine the max exponent
    * @param nr_f number of files
    * @param ind pointer to current index in the files vector
    * @param map_barrier barrier used inside the mapper function
    * @param map_mutex mutex used inside the mapper function
    * @param start_reducers pointer to variable that lets reducers start
    * */
    mapper(int number, queue<string> *string_queue, int nr_reducers,
            int nr_f, pthread_barrier_t *map_barrier,
            pthread_mutex_t *map_mutex, int *start_reducers) {
        id = number;
        file_queue = string_queue;
        max_exponent = nr_reducers + 1;
        nr_files = nr_f;
        barrier_map = map_barrier;
        mutex_map = map_mutex;
        mapper_done = start_reducers;

        // resize the exponent list to match the number of reducers
        exponent_lists.resize(nr_reducers);
    }


    /**
     * Function checking if the queue is empty or not
     * */
    int check_file_availability() {
        if (!file_queue->empty()) {
            return 1;
        }

        return 0;
    }

    /**
     * Function returning the next file at the top of the queue
     * */
    string get_file() {
        return file_queue->front();
    }

    /**
     * Function using binary search to find a value that raised to the given
     * power is equal to the given number
     * @param exponent exponent used for the binary search
     * @param number searched value
     * @param left starting value
     * @param right end value
     * */
    int binarySearchExponent(int exponent, long long int number,
                            long long int left, long long int right) {
        // check if square number
        if (exponent == 2 && right * right == number) {
            return right;
        }

        long long int middle;
        long long int power;

        while (left <= right) {
            // calculate middle and power
            middle = (left + right) / 2;
            power = pow(1.0 * middle, 1.0 * exponent);

            // check if value was found
            if (power == number) {
                return power;
            } else if (power > number) {
                // search to the left
                right = middle - 1;
            } else {
                // search to the right
                left = middle + 1;
            }
        }

        // value was not found
        return 0;
    }

    /**
     * Function processing a file, extracting numbers line by line and
     * inserting each number in the exponent list
     * @param filename name of file to be processed
     * */
    void process_file_with_bin_search(string filename) {
        // open file
        ifstream file;
        file.open(filename);

        string line;

        // read the number of vaues inside file
        if (!std::getline(file, line)) {
            // nothing could be read
            cout << "No numbers in file\n";
        }

        // read numbers inside the file
        while(std::getline(file, line)) {
            // convert to long long int
            long long int number = stoll(line);

            if (number == 1) {
                // 1 should be put in each exponent list
                for (int i = 0; i < max_exponent - 1; i++) {
                    exponent_lists[i].insert(1);
                }
            } else {
                // perform binary search for every exponent
                for (int i = 2; i <= max_exponent; i++) {
                    int div = binarySearchExponent(i, number, 2, sqrt(number));

                    // value was confirmed to match exponent
                    if (div) {
                        // insert value in the corresponding list
                        exponent_lists[i - 2].insert(number);
                    }
                }
            }
        }

        // close file
        file.close();
    }

    /**
     * Function calculating the divisor power
     * @param number value checked to be a perfect power
     * @param divisor divisor of number
     * @param pow_value power value of divisor
     * */
    int calculatePower(long long int number, long long int divisor,
                        long long int pow_value, int max_exponent) {
        int power = 2;

        // keep multiplying the power value until it matches the number
        while (pow_value < number && power < max_exponent) {
            // increase the power
            power++;
            // multiply current power value by the given divisor
            pow_value *= divisor;
        }

        // check if the power value is equal to the given number 
        if (pow_value == number) {
            return power;
        }

        return -1;
    }

    /**
     * Function processing a file, extracting numbers line by line and
     * inserting each perfect power in the exponent list
     * @param filename name of file to be processed
     * */
    void process_file(string filename) {
        // open file
        ifstream file;
        file.open(filename);

        string line;

        // read the number of vaues inside file
        if (!std::getline(file, line)) {
            // nothig to be read
            cout << "No numbers in file\n";
        }

        // read numbers inside the file
        while(std::getline(file, line)) {
            // convert to long long int
            long long int number = stoll(line);
            long long int power;

            if (number == 1) {
                // 1 should be put in each exponent list
                for (int i = 0; i < max_exponent - 1; i++) {
                    exponent_lists[i].insert(1);
                }
            }

            // iterate through number's divisors
            for (int i = 2; i * i <= number; i++) {
                // calculate square
                long long int pow2 = i * i;

                // check if number fas  a square as a divisor
                if (number % pow2 == 0) {
                    // search for exponent
                    power = calculatePower(number, i, pow2, max_exponent);

                    // exponent was found
                    if (power != -1) {
                        // insert number in the exponents list
                        exponent_lists[power - 2].insert(number);
                    }
                }
            }
        }

        // close the file
        file.close();
    }

    /**
    * Function printing the mapper's exponent lists
    * */
    void print_exponent_lists() {
        cout << "Exponent lists of Mapper " << id << "\n";

        // iterate through the exponents
        for (int i = 0; i < max_exponent - 1; i++) {
            cout << "Exponent " << i + 2 << ":\n";
            set<long long int>::iterator it;

            // iterate through the list corresponding to the exponent
            if (exponent_lists[i].size() != 0) {
                for (it = exponent_lists[i].begin();
                        it != exponent_lists[i].end(); it++) {
                    cout << *it << ' ';
                }
            } else {
                // noting inside the exponent's list
                cout << "Nothing to print";
            }

            cout << "\n\n";
        }
    }
};