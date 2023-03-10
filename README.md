# APD-Tema1

## Overview:
This project represents the implementation of a Map-Reduce model. The Mapper first processes the files, whose names are taken from an input file. When handling these files, the mapper searches for perfect powers and keeps them in an exponent lists. If there are more mappers, they have to operate in parallel and split the workload (the files) between them.  
Once the mappers finish, the reducers become active. Each reducer corresponds to an exponent and they will only look at the lists corresponding to that power. The reducers will count the unique numbers from the mappers' lists and then put the result in an output file.

## Structures:
The mapper has an id, a queue of files, a pointer to the variable that activates the reducers. It also keeps the mutex that will delimit the critical section and a barrier to synchronize all mappers, as well as the number of reducers, in order to determine the maximum exponent.  
The reducer also has an id, in order to calculate the exponent attributed to it, a vector of mappers, a pointer to the variable that informs it when the mappers are done, and it keeps the number of mappers. The reducer holds an exponent list where it will store all the numbers from the mappers' lists that match that exponent. It also has a variable where the number of unique numbers will be saved.

## Mapper:
The mapper uses a queue of files, and extracts the file at the front of the queue. A mutex is used when extracting a file from the queue so that the top of the queue does not look different for each mapper. When handling a file, the mapper performs a binary search for every number read based on the exponents. Once a number is taken from the file, the mapper iterates through the exponents and searches for an integer that, raised to the current exponent is equal to the number's value. If something is found, then the number is put in the exponent's corresponding list. Each exponent list is actually represented as a set in order to get rid of duplicates and minimize the reducers' workload. Once a mappers finishes processing the available files to it, a barrier will make it wait for the others to catch up. Thus, when all the mappers finish, the barriers will let them exit the mapper function. The variable signaling the state of the reducers will also be incremented after the barrier, meaning that the reducers can now become active.

## Reducer:
The reducers are stuck in a while loop until the mappers mappers finish. After, each reducer extracts the set attributed to its the reducer's corresponding exponent, and puts these values in another set.  At the end of this step, the reducer will have a set comprised of all the numbers found by all the mappers for that given exponent. Then, the reducer writes the size of the set, representing the unique values, inside an output file.
