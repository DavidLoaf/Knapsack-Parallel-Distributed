#include "../core/utils.h"
#include "../test/test.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <cassert>
#include <mpi.h>

#define DEFAULT_NUMBER_OF_THREADS "1"

// MACRO for better readability
#define DP(i, j) dp[(i) * (capacity+1) + (j)]

int world_size;
int world_rank;

int knapsack_distributed(const std::vector<Item> &items, int capacity)
{
  timer total_runtime;
  total_runtime.start();
  
  // define number of items
  int n = items.size();
  
  // define dynamic programming table
  int *dp = new int[2 * (capacity+1)]();

  //define capacity index ranges for processes:
  int base_range = capacity / world_size;
  int remainder = capacity % world_size;
  std::vector<int> indeces(world_size + 1, 1);
  indeces[world_size] = capacity + 1;

  for(int i = 1; i < world_size; i++)
  {
    indeces[i] = indeces[i-1] + base_range;

    if(remainder >= 1)
    {
      indeces[i] += remainder;
      remainder--;
    }
  }
  
  // Begin main algorithm
  timer t1;
  t1.start();

  int i;
  for (i = 1; i <= n; i++)
  {
    int top = i % 2;
    int bottom = top != 1;

    for (int j = indeces[world_rank]; j < indeces[world_rank+1]; j++)
    {
      const int weight = items[i-1].weight;
      const int value = items[i-1].value;
      const int prev = DP(bottom, j);

      if (weight <= j)
      {
        //dp[i][j] = std::max(dp[i-1][j], dp[i-1][j - items[i-1].weight] + items[i-1].value);
        DP(top, j) = std::max(
          prev,
          DP(bottom, j-weight) + value
        );
      }
      else
      {
        //dp[i][j] = dp[i-1][j];
        DP(top, j) = prev;
      }
    }

    if(world_rank != 0)
    {
      MPI_Recv(&DP(top, 0), indeces[world_rank], MPI_INT, world_rank-1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if(world_rank != world_size - 1)
    {
      MPI_Send(&DP(top, 0), indeces[world_rank + 1], MPI_INT, world_rank + 1, i, MPI_COMM_WORLD);
    }
  }

  double runtime = t1.stop();

  int index = i % 2;
  int max_value;
  int value = DP(index, capacity);
  MPI_Reduce(&value, &max_value, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  double total_time = total_runtime.stop();

  double* times = new double[world_size];

  MPI_Gather(&runtime, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);


  if(world_rank == 0)
  {
     std::cout << "Process ID --- Runtime (s)" << std::endl;
    for(int i = 0; i < world_size; i++)
    {
      std::cout << std::setw(10) << i << " --- " << std::setw(11) << times[i] << std::endl;
    }
  }

  if(world_rank == 0)
  {
    std::cout << "\nMaximum value achievable: " << max_value << std::endl;
    std::cout << "Total runtime: " << total_time << " seconds" << std::endl;
  }

  delete[] dp;
  
  return max_value;
}


int main(int argc, char **argv)
{
  MPI_Init(NULL, NULL);
  
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

      cxxopts::Options options("Knapsack_Distributed", "Distributed implementation of 0/1 knapsack problem");
    
    options.add_options()
        ("n", "Number of items", cxxopts::value<int>()->default_value("1000000"))
        ("c", "Knapsack capacity", cxxopts::value<int>()->default_value("1000"))
        ("h,help", "Print usage")
        ("t", "Run tests", cxxopts::value< bool >()->default_value("false"));
        
    auto result = options.parse(argc, argv);
    
    if(result.count("help")) 
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    
    // Get parameters
    int n = result["n"].as<int>();
    int capacity = result["c"].as<int>();
    bool run_tests = result["t"].as< bool >();
    
    // run test:
    if (run_tests)
    {
        std::cout << std::endl;
        std::cout << "TESTING" << std::endl;
        std::cout << std::endl;
        test(knapsack_distributed);

        MPI_Finalize();

        return 0;
    }
    
    // Create sample items for testing
    std::vector< Item > items;

    if(world_rank == 0)
    {
      std::cout << "\nGenerating " << n << " random items..." << std::endl;
    }
    // Generate random items
    srand(n);
    for(int i = 0; i < n; i++) 
    {
        int w = rand() % (capacity/2) + 1;  // weight between 1 and capacity/2
        int v = rand() % 100 + 1;  // value between 1 and 100
        items.push_back(Item(w, v));
    }
    
    if(world_rank == 0)
    {
      std::cout << "Number of Processes: " << world_size << std::endl;
      // Print items
      std::cout << "\nItems available:" << n << std::endl;
      std::cout << "Knapsack capacity: " << capacity << std::endl;
    }

    knapsack_distributed(items, capacity);

    MPI_Finalize();

    return 0;
  
}

#undef DP