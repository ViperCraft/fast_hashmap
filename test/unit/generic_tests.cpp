#include "../../fast_hashmap.hpp"
#include "gtest/gtest.h"
#include <unordered_set>
#include <iostream>

// return number of errors
template<typename T, typename Z>
static size_t test( T &cont, Z max_id, size_t count )
{
    srand(336);
    size_t errors = 0;
    std::unordered_set<Z> good;
    for( size_t i = 0; i < count; ++i )
    {
        Z id = rand() % max_id;
        if( good.insert(id).second != cont.insert(id) )
        {
            //std::cerr << "error on id=" << id << std::endl;
            ++errors;
        }
    }
    return errors;
}

TEST(Insert32, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset30<2, 8> fhs(count);
        EXPECT_EQ(0UL, test(fhs, 1000000U, count));
    }
}

TEST(Insert64, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset62<2, 8> fhs(count);
        EXPECT_EQ(0UL, test(fhs, 100000000000UL, count));
    }
}

TEST(AfterClear, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset30<3, 4> fhs(count);
        for( uint32_t i = 0; i < 3; ++i )
        {
            EXPECT_EQ(0UL, test(fhs, 1000000U, count));
            fhs.clear();
        }
    }
}
