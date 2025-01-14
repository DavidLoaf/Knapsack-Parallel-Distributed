#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

#include "../core/cxxopts.h"
#include "../core/utils.h"
#include "../test/test.h"

// Macro to simplify Dynamic programming traversal
#define DP(i, j) thread->dp[(i) * (thread->capacity+1) + (j)]

// object to handle thread data
class ThreadData
{
    public: 

    const std::vector<Item>* items;
    std::atomic<int>*** rowCheck;
    int* dp;
    int start;
    int end;
    int capacity;
    double time;
    uint32_t id;
};

void parallel_knapsack_function(void* _arg)
{
    
    timer t;   
    t.start();

    ThreadData* thread = (ThreadData*)_arg;
    int n = thread->items->size();
    (*thread->rowCheck)[0][thread->id] = 1;

    for (int i = 1; i <= n; i++)
    {
        
        if (thread->id != 0) 
        {
            while ((*thread->rowCheck)[i-1][thread->id-1].load() != 1); // Block 
        }

        for (int j = thread->start; j <= thread->end; j++)
        {
            const int weight = (*thread->items)[i-1].weight;
            const int value = (*thread->items)[i-1].value;
            const int prev = DP(i-1, j);

            if (weight <= j)
            {
                //dp[i][j] = std::max(dp[i-1][j], dp[i-1][j - items[i-1].weight] + items[i-1].value);
                DP(i, j) = std::max(
                    prev,
                    DP(i-1, j-weight) + value
                );
            }
            else
            {
                //dp[i][j] = dp[i-1][j];
                DP(i, j) = prev;
            }
        }
        (*thread->rowCheck)[i][thread->id].store(1);
    }

    thread->time = t.stop();
}

// used pseudocode from:
// https://en.wikipedia.org/wiki/Knapsack_problem#0-1_knapsack_problem
int knapsack_parallel_setup(const std::vector<Item> &items, int capacity, uint32_t nThreads) 
{
    // num items
    uint32_t n = items.size();

    // dynamic programing table
    int* dp = new int[(n+1) * (capacity+1)]();

    // Create threads
    std::vector<std::thread> threads(nThreads);

    // initializing thread data objects
    std::vector<ThreadData> data(nThreads);

    // initialize rowcheck 
    std::atomic<int>** rowCheck = new std::atomic<int>*[n+1];
    
    for(int i = 0; i < n+1; i++)
    {
        rowCheck[i] = new std::atomic<int>[nThreads]();
    }
    
    // Divide work among threads
    uint32_t itemsPerThread = capacity / nThreads;
    uint32_t remainder = capacity % nThreads;
    
    data[0].start = 1;
    data[0].end = data[0].start + itemsPerThread - 1;

    for (uint32_t i = 1; i < nThreads; i++)
    {
        data[i].start = data[i-1].end + 1;
        data[i].end = data[i].start + itemsPerThread - 1;

        if(remainder >= 1)
        {
            data[i].end += 1;
            remainder--;
        }
    }

    // Begin execution timer:
    timer t;
    t.start();
    
    // ########################### PARALLEL CODE BEGINS ###########################
    for (uint32_t i = 0; i < nThreads; i++)
    {
        data[i].dp = dp;
        data[i].items = &items;
        data[i].rowCheck = &rowCheck;
        data[i].capacity = capacity;
        data[i].id = i;

        threads[i] = std::thread(
            parallel_knapsack_function, 
            &data[i]
        );
    }
    
    // Join threads
    for (uint32_t i = 0; i < nThreads; i++)
    {
        threads[i].join();
    }
    // ############################ PARALLEL CODE ENDS ############################
    
    // End timer
    double runtime = t.stop();

    // Print statistics
    std::cout << "Thread ID --- Runtime (s)"  << std::endl;
    for (uint32_t i = 0; i < nThreads; i++)
    {
        std::cout << std::setw(9) << i << " --- " << std::setw(11) << data[i].time << std::endl;
    }

    // load final value
    int final_value = dp[n * (capacity+1) + capacity];
    
    // print total runtime and max value.
    std::cout << "\nMaximum value achievable: " << final_value << std::endl;
    std::cout << "Total runtime: " << runtime << " seconds" << std::endl;

    // memory leak prevention
    delete[] dp;

    return final_value;
}

int main(int argc, char **argv)
{
    cxxopts::Options options("Knapsack_Parallel", "Serial implementation of 0/1 knapsack problem");
    
    options.add_options()
        ("nThreads", "Number of threads", cxxopts::value<uint32_t>()->default_value("1"))
        ("n", "Number of items", cxxopts::value<int>()->default_value("100000"))
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
    uint32_t nThreads = result["nThreads"].as<uint32_t>();
    bool run_tests = result["t"].as< bool >();
    
    // run test:
    if (run_tests)
    {
        std::cout << std::endl;
        std::cout << "TESTING" << std::endl;
        std::cout << std::endl;
        test_threads(knapsack_parallel_setup, nThreads);

        return 0;
    }
    
    // Create sample items for testing
    std::vector< Item > items;
    std::cout << "\nGenerating " << n << " random items..." << std::endl;
    
    // Generate random items
    srand(n);
    for(int i = 0; i < n; i++) 
    {
        int w = rand() % (capacity/2) + 1;  // weight between 1 and capacity/2
        int v = rand() % 100 + 1;  // value between 1 and 100
        items.push_back(Item(w, v));
    }
    

    // Print item, thread, capacity details.
    std::cout << "\nItems available:" << n << std::endl;
    std::cout << "Knapsack capacity: " << capacity << std::endl;
    std::cout << "Number of Threads: " << nThreads << std::endl;
    
    knapsack_parallel_setup(items, capacity, nThreads);
    
    return 0;
}

#undef DP