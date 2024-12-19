# CMPT-431-Project (Group 22)
For our final project we are implementing a solution for the 0/1 Knapsack problem using dynamic programming. We are implementing a serial version, a parallel version using threads, and a distributed version using MPI.

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

# How to run the python script on Slurm:
1. Copy folder to your slurm home directory.
2. Run `make` to compile the program
3. edit config to align with the details of your current directory and user information (lines 2 and 3)
4. run python3 generate_sbatch.py

# Group Members:
- David Krljanovic #301427415
- HoJun Lee #301359952
- Shabbir Yusufali #301424687

# Citations:
- Used the professor's provided cxxopts library for parsing command line arguments
- Got the pseudocode for the algorithm from the book "Introduction to Parallel Computing" by Ananth Grama et al.
- Referenced the professor's Makefile to create our own Makefile
- Referenced previous assignments to create the structure of the program, as well as how to run MPI programs
- The testing script was borrowed from another group (Group 40), but we modified it to fit our program
