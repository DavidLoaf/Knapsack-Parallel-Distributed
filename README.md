# A Project done For CMPT 431

**ALL TEST RESULTS WERE DETERMINED ON THE SFU SLURM CLUSTER THAT WE USED FOR COURSE ASSIGNMENTS**

# Pre-requisites:
- C++ Compiler
- OpenMPI
- Make

# How to compile and run the program on any computer:
1. Create a build directory in the root of the project
2. Run `make` to compile the program
3. Run either:
	- `./build/knapsack_serial -n <number of items> -c <capacity>` to run the serial version of the program
	- `./build/knapsack_parallel -n <number of items> -c <capacity> --nThreads <number of threads>` to run the parallel version of the program
	- `mpirun -np <number of processes> ./build/knapsack_distributed -n <number of items> -c <capacity>` to run the distributed MPI version of the program
4. Run `make clean` to clean up the build files

