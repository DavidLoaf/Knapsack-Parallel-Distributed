#include "../core/utils.h"
#include "../test/test.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <cassert>
#include <mpi.h>

#define DEFAULT_NUMBER_OF_THREADS "1"

int world_size;
int world_rank;

int knapsack_distributed(const std::vector<Item> &items, int capacity)
{

  int n = items.size();

  // calculate the amount of work each process will do
  std::vector<int> items_per_process(world_size, 0);

  
  int base_items_per_process = n / world_size;
  int remainder = n % world_size;
  
  
  
  for(int i = 0; i < world_size; i++)
  {
    if(remainder > 0)
    {
      items_per_process[i] = base_items_per_process + 1;
      remainder--;
    }
    else
    {
      items_per_process[i] = base_items_per_process;
    }
  }

  int starting_item = 0;
  for(int i = 0; i < world_rank; i++)
  {
    starting_item += items_per_process[i];
  }

  int chunk_size = capacity / 5;
  
  // dynamic programing table
  // Each process only needs the top row from the previous row.
  //std::vector< std::vector< int >> dp(items_per_process[world_rank] + 1, std::vector<int>(capacity + 1, 0));
  int *dp = new int[(items_per_process[world_rank]+1)*(capacity+1)]();

  // MACRO for better readability
  #define DP(i, j) dp[(i) * (capacity+1) + (j)]

  timer t1;
  t1.start();

  // ##################################### BEGIN PARALLEL CODE #####################################
  for (int j = 1; j <= capacity; j++)
  {
    
    int starting_j = j;

    if(world_rank != 0)
    {
      //MPI_Recv(&dp[0][starting_j], chunk_size, MPI_INT, world_rank-1, starting_j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(
        &DP(0, starting_j),
        chunk_size,
        MPI_INT,
        world_rank-1,
        starting_j,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
      );
    }
    // k loop for chunk size.
    for(int k = 0; k < chunk_size; k++)
    {
      // inner loop will do dp[i][start]......dp[i][stop]
      for (int i = 1; i <= items_per_process[world_rank]; i++) 
      {

        Item current_item = items[starting_item + i - 1];
        int weight = current_item.weight;
        int value = current_item.value; 

        if (items[starting_item + i - 1].weight <= j)
        {
          //dp[i][j] = std::max(dp[i-1][j], dp[i-1][j - weight] + value);
          DP(i, j) = std::max(
            DP(i-1, j),
            DP(i-1, j-current_item.weight) + current_item.value
          );
        }
        else
        {
          //dp[i][j] = dp[i-1][j];
          DP(i, j) = DP(i-1, j);
        }

      }

 // increment j insisde k loop
      j++;
    }
    j--;
  //int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
    if(world_rank != world_size - 1)
    {
      //MPI_Send(&dp[items_per_process[world_rank]][starting_j], chunk_size, MPI_INT, world_rank + 1, starting_j, MPI_COMM_WORLD);
      MPI_Send(
        &DP(items_per_process[world_rank], starting_j),
        chunk_size,
        MPI_INT,
        world_rank+1,
        starting_j,
        MPI_COMM_WORLD
      );
    }
  }
  
  //int value = dp[items_per_process[world_rank]][capacity];
  int value = DP(items_per_process[world_rank], capacity);
  int max_value;

  MPI_Reduce(&value, &max_value, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
  delete[] dp;

  double runtime = t1.stop();

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
    std::cout << "Total runtime: " << runtime << " seconds" << std::endl;
  }
  
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