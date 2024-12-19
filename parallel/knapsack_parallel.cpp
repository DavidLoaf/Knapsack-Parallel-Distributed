#include <iostream>
#include <vector>
#include <thread>

#include "../core/cxxopts.h"
#include "../core/utils.h"
#include "../test/test.h"


void row_knapsack_function(const std::vector<Item> &items, int capacity, int *dp, int start, int end, CustomBarrier &barrier, double *time, int i)
{
    timer t;
    t.start();
    int n = items.size();

    #define DP_ROW(i, j) dp[(i) * (capacity+1) + (j)]

    for (int i = 1; i <= n; i++)
    {
        for (int j = start; j <= end; j++)
        {
            const int weight = items[i-1].weight;
            const int value = items[i-1].value;
            const int prev = DP_ROW(i-1, j);

            if (items[i-1].weight <= j)
            {
                //dp[i][j] = std::max(dp[i-1][j], dp[i-1][j - items[i-1].weight] + items[i-1].value);
                DP_ROW(i, j) = std::max(
                    prev,
                    DP_ROW(i-1, j-weight) + value
                );
            }
            else
            {
                //dp[i][j] = dp[i-1][j];
                DP_ROW(i, j) = prev;
            }
        }
        barrier.wait();
    }
    *time = t.stop();
}

void column_knapsack_function(const std::vector<Item> &items, int capacity, int *dp, int start, int end, CustomBarrier &barrier, double *time, int threadNum, int nThreads)
{
    timer t;
    t.start();
    int n = items.size();
    int overflow = 0;

    #define DP_COL(j, i) dp[(j) * (n+1) + (i)]
    
    // stall threads while other work is being done to eliminate communication based on dependencies. 
    while(overflow < threadNum)
    {
        barrier.wait();
        overflow++;
    }
    for (int j = 1; j <= capacity; j++)
    {
        
        for (int i = start; i <= end; i++)
        {
            // caching stuff.
            const int weight = items[i-1].weight;
            const int value = items[i-1].value;
            const int prev = DP_COL(j, i-1);
            
            if (items[i-1].weight <= j)
            {
                
                //dp[j][i] = std::max(dp[j][i-1], dp[j - items[i-1].weight][i-1] + items[i-1].value);
                DP_COL(j, i) = std::max(
                    prev,
                    DP_COL(j-weight, i-1)+value
                );
            }
            else
            {
                //dp[j][i] = dp[j][i-1];
                DP_COL(j, i) = prev;
            }
        }
        barrier.wait();
    }

    while(overflow < nThreads -1)
    {
        barrier.wait();
        overflow++;
    }
    *time = t.stop();
}

// used pseudocode from:
// https://en.wikipedia.org/wiki/Knapsack_problem#0-1_knapsack_problem
int knapsack_parallel(const std::vector< Item > &items, int capacity, uint32_t nThreads) 
{
    int n = items.size();

    // dynamic programing table
    //std::vector<std::vector<int>> dp;
    int *dp;

    // Create a barrier
    CustomBarrier barrier(nThreads);

    // Create threads
    std::vector<std::thread> threads(nThreads);
    std::vector<int> start(nThreads);
    std::vector<int> end(nThreads);
    std::vector<double> times(nThreads, 0);

    int itemsPerThread;
    int remainder;

    // We will chose to change how we iterate through the DP table based on spatial locality access.
    // rows are items, columns are weights
    if(capacity < n)
    {
        //dp = std::vector< std::vector< int >> (capacity+1, std::vector< int >(n+1, 0));
        dp = new int[(capacity+1) * (n+1)]();
        itemsPerThread = n / nThreads;
        remainder = n % nThreads;
    }
    // columns are items, rows are weights. 
    else
    {
        //dp = std::vector< std::vector< int >> (n+1, std::vector< int >(capacity+1, 0));
        dp = new int[(n+1) * (capacity+1)]();
        itemsPerThread = capacity / nThreads;
        remainder = capacity % nThreads;
    }

    // Divide work among threads
    for (uint32_t i = 0; i < nThreads; i++)
    {
        if(i == 0)
        {
            start[i] = 1;
            end[i] = nThreads > 1 ? itemsPerThread + remainder: itemsPerThread;
        }
        else
        {
            if(nThreads <= n)
            {
                start[i] = end[i-1] + 1;
                end[i] = start[i] + itemsPerThread - 1;

                if(capacity < n)
                {
                    if(end[i] > n)  // Cap end[i] to 'n'
                    {
                        end[i] = n;
                    }
                }
            }
            else
            {
                start[i] = 1;
                end[i] = 0;
            }
        }
    }

    // Time the execution
    timer t;
    t.start();
    
    if(capacity < n)
    {
        for (uint32_t i = 0; i < nThreads; i++)
        {
            threads[i] = std::thread(
                column_knapsack_function, 
                std::ref(items), 
                capacity, 
                std::ref(dp), 
                start[i], 
                end[i], 
                std::ref(barrier), 
                &times[i], 
                i, 
                nThreads
            );
        }
    }
    else
    {
        for (uint32_t i = 0; i < nThreads; i++)
        {
            threads[i] = std::thread(
                row_knapsack_function, 
                std::ref(items), 
                capacity, 
                std::ref(dp), 
                start[i], 
                end[i], 
                std::ref(barrier), 
                &times[i], 
                i
            );
        }
    }
    // Join threads
    for (uint32_t i = 0; i < nThreads; i++)
    {
        threads[i].join();
    }

    double runtime = t.stop();

    std::cout << "Thread ID --- Runtime (s)"  << std::endl;

    for (uint32_t i = 0; i < nThreads; i++)
    {
        std::cout << std::setw(9) << i << " --- " << std::setw(11) << times[i] << std::endl;
    }

    
    int final_value;
    if(capacity < n)
    {
        final_value = DP_COL(capacity, n);
    }
    else
    {
        final_value = DP_ROW(n, capacity);
    }

    delete[] dp;

    
    std::cout << "\nMaximum value achievable: " << final_value << std::endl;
    std::cout << "Total runtime: " << runtime << " seconds" << std::endl;

    return final_value;

    /*
    if(capacity < n)
    {
        return dp[capacity][n];
    }
    else
    {
        return dp[n][capacity];
    }
    */
}

int main(int argc, char **argv)
{
    cxxopts::Options options("Knapsack_Parallel", "Serial implementation of 0/1 knapsack problem");
    
    options.add_options()
        ("nThreads", "Number of threads", cxxopts::value<uint32_t>()->default_value("1"))
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
    uint32_t nThreads = result["nThreads"].as<uint32_t>();
    bool run_tests = result["t"].as< bool >();
    
    // run test:
    if (run_tests)
    {
        std::cout << std::endl;
        std::cout << "TESTING" << std::endl;
        std::cout << std::endl;
        test_threads(knapsack_parallel, nThreads);

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
    
    // Print items
    // Print items
    std::cout << "\nItems available:" << n << std::endl;
    std::cout << "Knapsack capacity: " << capacity << std::endl;
    std::cout << "Number of Threads: " << nThreads << std::endl;
    
    knapsack_parallel(items, capacity, nThreads);
    
    return 0;
}

#undef DP