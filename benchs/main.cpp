#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <iomanip>
#include "timestamp.hpp"
#include "../fast_hashmap.hpp"
#include <nmmintrin.h>

inline size_t get_aligned( size_t count, size_t align )
{
    if( count & ( align - 1 ) )
        count = (count + align) & ~( align - 1 );
    return count;
}

inline void* aligned_malloc( size_t sz, size_t alignment = 16 )
{
    void *out_bytes;
    if( posix_memalign(&out_bytes, alignment, sz) )
        throw std::runtime_error("aligned_malloc: failed to allocate!");

    return out_bytes;
}

struct std_uset
{
    std::unordered_set<uint32_t> s;

    std_uset( size_t count )
    {
        s.reserve(count);
    }

    bool insert( uint32_t id )
    {
        return s.insert(id).second;    
    }

    void clear()
    {
        s.clear();
    }

    size_t count() const { return s.size(); }
};

struct bitmap_set
{
    bitmap_set( size_t max_set ) : bm(max_set), count_(0) {}

    bool insert( uint32_t id )
    {
        if( bm[id] )
            return false;
        bm[id] = 1;
        ++count_;
        return true;
    }

    size_t count() const { return count_; }

    void clear()
    {
        count_ = 0;
        bm.clear();
    }

    std::vector<bool> bm;
    size_t count_;
};

struct bitmap_set_fclear
{
    bitmap_set_fclear( size_t max_capacity )
        : count_(0)
        , capacity_( get_aligned(max_capacity, 256) / 8 )
        , bitmap_( static_cast<uint64_t*>(aligned_malloc(capacity_, 32)) )
        {
            memset(bitmap_, 0, capacity_);
        }

    ~bitmap_set_fclear()
    {
        free(bitmap_);
    }

    bool is_marked( uint32_t docid ) const
    {
        uint64_t const w = bitmap_[ docid / 64U ];
        return ((w >> (docid % 64U)) & 1UL) == 1UL;
    }

    bool insert( uint32_t docid )
    {
        if( is_marked(docid) )
        {
            return false;
        }
        bitmap_[docid / 64U] |= 1UL << (docid % 64U);
        ++count_;
        return true;
    }
    
    size_t count() const { return count_; }

    void clear()
    {
        count_ = 0;
        //zero_mem(bitmap_, capacity_);
        memset(bitmap_, 0, capacity_);
    }

    size_t count_, capacity_;
    uint64_t *bitmap_;
    
};


template<typename T>
void benchmark( T &cont, uint32_t max_id, size_t count )
{
    srand(336);
    std::vector<uint32_t> values;
    values.reserve(count);
    for( size_t i = 0; i < count; ++i )
    {
        uint32_t id = rand() % max_id;
        values.push_back(id);
    }

    Timestamp ts;
    for( size_t i = 0; i < 1000; ++i )
    {
        for( auto v : values )
            cont.insert(v);

        cont.clear();
    }

    auto emills = ts.elapsed_millis();

    std::cout << "  for count=" << count 
              << " max_value=" << max_id
              << " type=" << typeid(T).name() 
              << " => " << emills << " millis." << std::endl; 
}


int main()
{
    {
        std::cout << "Perform insert and clear tests for several container types.\n";
        uint32_t max_id = 10000000;
        for( size_t count : { 1000, 20000, 200000 } )
        {
            std_uset gs(count);
            benchmark(gs, max_id, count);
            bitmap_set bm(max_id);
            benchmark(bm, max_id, count);
            bitmap_set_fclear bm2(max_id);
            benchmark(bm2, max_id, count);
            fast_hashset30<2, 8> fhs(count);
            benchmark(fhs, max_id, count);
            fast_hashset_fclear<uint32_t, utils::hash32, 2, 8> fhs2(count);
            benchmark(fhs2, max_id, count);
        }
    }

    

    return 0;
}