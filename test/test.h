#include <iostream>
#include <vector>
#include "../core/utils.h"
#include "../core/item.h"

void test(int (*function)(const std::vector< Item > &items, int capacity)) 
{
    int testNum = 1;
    int passed = 0;
    
    // Test 1: Basic test with small numbers
    {
        std::vector<Item> items = {Item(2, 3), Item(3, 4), Item(4, 5), Item(5, 6)};
        int capacity = 10;
        int result = function(items, capacity);
        int expected = 13;
        
        std::cout << "Test " << testNum++ << ": Basic test with small numbers - ";
        if(result == expected)
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 2: Empty knapsack
    {
        std::vector<Item> items = {Item()};
        int capacity = 10;
        int result = function(items, capacity);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Empty knapsack - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else 
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 3: Zero capacity
    {
        std::vector<Item> items = {Item(2, 3), Item(3, 4)};
        int capacity = 0;
        int result = function(items, capacity);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Zero capacity - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 4: Items too heavy for capacity
    {
        std::vector<Item> items = {Item(10, 20), Item(15, 30)};
        int capacity = 5;
        int result = function(items, capacity);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Items too heavy for capacity - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 5: Exact capacity match
    {
        std::vector<Item> items = {Item(5, 10), Item(5, 12)};
        int capacity = 5;
        int result = function(items, capacity);
        int expected = 12;
        
        std::cout << "Test " << testNum++ << ": Exact capacity match - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }
    
    // Test 6: All items have same weight but have different values
    {
        std::vector<Item> items =  {Item(5, 10), Item(5, 15), Item(5, 20), Item(5,25)};
        int capacity = 10;
        int result = function(items, capacity);
        int expected = 45;
        
        std::cout << "Test " << testNum++ << ": Items with same weight but different values - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 7 : Only one item is under capacity
    {
        std::vector<Item> items =  {Item(10, 20), Item(15, 30), Item(20, 40)};
        int capacity = 11;
        int result = function(items, capacity);
        int expected = 20;
        
        std::cout << "Test " << testNum++ << ": Only one item fits - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 8 : Very large capacity with small items
    {
        std::vector<Item> items =  {Item(1, 1), Item(2, 2), Item(3, 3)};
        int capacity = 1000;
        int result = function(items, capacity);
        int expected = 6;
        
        std::cout << "Test " << testNum++ << ": Very large capacity with small items - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // test 9 : Multiple identical items
    {
        std::vector<Item> items =  {Item(5, 10), Item(5, 10), Item(5, 10)};
        int capacity = 10;
        int result = function(items, capacity);
        int expected = 20;
        
        std::cout << "Test " << testNum++ << ": Multiple identical items - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }
}



void test_threads(int (*function)(const std::vector< Item > &items, int capacity, uint32_t nThreads), uint32_t nThreads) 
{
    int testNum = 1;
    int passed = 0;
    
    // Test 1: Basic test with small numbers
    {
        std::vector<Item> items = {Item(2, 3), Item(3, 4), Item(4, 5), Item(5, 6)};
        int capacity = 10;
        int result = function(items, capacity, nThreads);
        int expected = 13;
        
        std::cout << "Test " << testNum++ << ": Basic test with small numbers - ";
        if(result == expected)
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 2: Empty knapsack
    {
        std::vector<Item> items = {Item()};
        int capacity = 10;
        int result = function(items, capacity, nThreads);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Empty knapsack - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else 
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 3: Zero capacity
    {
        std::vector<Item> items = {Item(2, 3), Item(3, 4)};
        int capacity = 0;
        int result = function(items, capacity, nThreads);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Zero capacity - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 4: Items too heavy for capacity
    {
        std::vector<Item> items = {Item(10, 20), Item(15, 30)};
        int capacity = 5;
        int result = function(items, capacity, nThreads);
        int expected = 0;
        
        std::cout << "Test " << testNum++ << ": Items too heavy for capacity - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 5: Exact capacity match
    {
        std::vector<Item> items = {Item(5, 10), Item(5, 12)};
        int capacity = 5;
        int result = function(items, capacity, nThreads);
        int expected = 12;
        
        std::cout << "Test " << testNum++ << ": Exact capacity match - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }
    
    // Test 6: All items have same weight but have different values
    {
        std::vector<Item> items =  {Item(5, 10), Item(5, 15), Item(5, 20), Item(5,25)};
        int capacity = 10;
        int result = function(items, capacity, nThreads);
        int expected = 45;
        
        std::cout << "Test " << testNum++ << ": Items with same weight but different values - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 7 : Only one item is under capacity
    {
        std::vector<Item> items =  {Item(10, 20), Item(15, 30), Item(20, 40)};
        int capacity = 11;
        int result = function(items, capacity, nThreads);
        int expected = 20;
        
        std::cout << "Test " << testNum++ << ": Only one item fits - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // Test 8 : Very large capacity with small items
    {
        std::vector<Item> items =  {Item(1, 1), Item(2, 2), Item(3, 3)};
        int capacity = 1000;
        int result = function(items, capacity, nThreads);
        int expected = 6;
        
        std::cout << "Test " << testNum++ << ": Very large capacity with small items - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }

    // test 9 : Multiple identical items
    {
        std::vector<Item> items =  {Item(5, 10), Item(5, 10), Item(5, 10)};
        int capacity = 10;
        int result = function(items, capacity, nThreads);
        int expected = 20;
        
        std::cout << "Test " << testNum++ << ": Multiple identical items - ";
        if(result == expected) 
            std::cout << "PASSED" << std::endl;
        else
            std::cout << "FAILED (Expected " << expected << ", got " << result << ")" << std::endl;
    }
}