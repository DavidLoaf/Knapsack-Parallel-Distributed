#include <iostream>
#include <vector>

#include "../core/cxxopts.h"
#include "../test/test.h"


// used pseudocode from:
// https://en.wikipedia.org/wiki/Knapsack_problem#0-1_knapsack_problem
int knapsack_serial(const std::vector< Item > &items, int capacity) 
{
    timer t1;
    t1.start();

    int n = items.size();

    // dynamic programing table
    //std::vector< std::vector< int >> dp(n+1, std::vector< int >(capacity+1, 0));
    int *dp = new int[(n+1) * (capacity+1)]();

    // MACRO so that the array indexing is more readable
    #define DP(i, j) dp[(i) * (capacity+1) + (j)]

    for (int i = 1; i <= n; i++)
    {
        for (int j = 0; j <= capacity; j++)
        {
            if (items[i-1].weight <= j)
            {
                //dp[i][j] = std::max(dp[i-1][j], dp[i-1][j - items[i-1].weight] + items[i-1].value);
                DP(i, j) = std::max(
                    DP(i-1, j),
                    DP(i-1, j-items[i-1].weight)+items[i-1].value
                );
            }
            else
            {
                //dp[i][j] = dp[i-1][j];
                DP(i, j) = DP(i-1, j);
            }
        }
    }

    int result = DP(n, capacity);
    delete[] dp;

    double runtime = t1.stop();

    std::cout << "\nMaximum value achievable: " << result << std::endl;
    std::cout << "Runtime: " << runtime << " seconds" << std::endl;

    return result;
}

int main(int argc, char **argv)
{
    cxxopts::Options options("Knapsack_Serial", "Serial implementation of 0/1 knapsack problem");
    
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
        test(knapsack_serial);

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
    std::cout << "\nItems available:" << n << std::endl;
    std::cout << "Knapsack capacity: " << capacity << std::endl;
    
    knapsack_serial(items, capacity);
    
    return 0;
}

#undef DP