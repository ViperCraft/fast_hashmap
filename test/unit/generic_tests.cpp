#include <unordered_set>
#include <iostream>
#include "../../fast_hashmap.hpp"
#include "gtest/gtest.h"

// return number of errors
template<typename T, typename Z>
static size_t test_insert( T &cont, Z max_id, size_t count )
{
    srand(336);
    size_t errors = 0;
    std::unordered_set<Z> good;
    for( size_t i = 0; i < count; ++i )
    {
        Z id = rand() % max_id;
        if( good.insert(id).second != cont.insert(id) )
            ++errors;
    }
    return errors;
}

TEST(Insert32, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset30<2, 8> fhs(count);
        EXPECT_EQ(0UL, test_insert(fhs, 1000000U, count));
    }
}

TEST(Insert64, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset62<2, 8> fhs(count);
        EXPECT_EQ(0UL, test_insert(fhs, 100000000000UL, count));
    }
}

TEST(BitmapInsert, TestIsTrue)
{
    size_t max_id = 1000000U;
    for( size_t count : { 1000, 10000, 200000} )
    {
        bitmap_fclear bm(max_id);
        EXPECT_EQ(0UL, test_insert(bm, max_id, count));
    }  
}

TEST(AfterClear, TestIsTrue)
{
    for( size_t count : { 1000, 10000, 200000} )
    {
        fast_hashset30<3, 4> fhs(count);
        for( uint32_t i = 0; i < 3; ++i )
        {
            EXPECT_EQ(0UL, test_insert(fhs, 1000000U, count));
            fhs.clear();
            EXPECT_EQ(0UL, fhs.count());
        }
    }
}

TEST(BitmapAfterClear, TestIsTrue)
{
    size_t max_id = 1000000U;
    for( size_t count : { 1000, 10000, 200000} )
    {
        bitmap_fclear bm(max_id);
        for( uint32_t i = 0; i < 3; ++i )
        {
            EXPECT_EQ(0UL, test_insert(bm, max_id, count));
            bm.clear();
            EXPECT_EQ(0UL, bm.count());
        }
    }
}


TEST(FindValues, TestIsTrue)
{
    srand(336);
    for( size_t count : { 1000, 10000, 200000} )
    {
        std::vector<uint32_t> values(count);

        for( uint32_t ord = 0; ord < 2; ++ord )
        {
            for( size_t i = 0; i < count; ++i )
                values[i] = ord ? i : rand() % 1000000U;

            fast_hashset30<2, 5> fhs(count);

            for( auto v : values )
                fhs.insert(v);

            // order as is
            for( size_t i = 0; i < count; ++i )
                EXPECT_EQ(true, fhs.find(values[i]));
        
            // random order
            for( size_t i = 0; i < count; ++i )
                EXPECT_EQ(true, fhs.find(values[rand() % count]));
        }
    }
}
