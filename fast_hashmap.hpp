#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <limits.h> // for CHAR_BIT
#include <type_traits>
#include <immintrin.h>

namespace utils {

inline size_t get_aligned( size_t count, size_t align )
{
    if( count & ( align - 1 ) )
        count = (count + align) & ~( align - 1 );
    return count;
}

template<typename T>
inline T rotl( T n, uint32_t c )
{
    static_assert( std::is_unsigned<T>::value );
    T const mask = (CHAR_BIT * sizeof(n) - 1);
    c &= mask;
    return (n<<c) | (n >> ( (-c) & mask ));
}

struct hash32
{
    uint32_t operator () ( uint32_t k ) const noexcept
    {
        return rotl(4111732 * k, 13);
    }

    uint32_t operator () ( uint64_t k ) const noexcept
    {
        return rotl(k, 29) ^ k;
    }
};

// fast zeroing block size of 64 bytes
inline void zero_mem64( void *mem, size_t capacity )
{
    size_t cap = capacity / 64;
    __m128i *store_ptr = (__m128i*)mem;
    __m128i z = _mm_setzero_si128();
    for( size_t i = 0; i < cap; ++i )
    {
        _mm_store_si128(store_ptr + i * 4 + 0, z);
        _mm_store_si128(store_ptr + i * 4 + 1, z);
        _mm_store_si128(store_ptr + i * 4 + 2, z);
        _mm_store_si128(store_ptr + i * 4 + 3, z);
    }
}

inline void* aligned_malloc( size_t sz, size_t alignment = 64 )
{
    void *out_bytes;
    if( posix_memalign(&out_bytes, alignment, sz) )
        throw std::runtime_error("aligned_malloc: failed to allocate!");

    return out_bytes;
}

} // namespace utils

// you can tune(node_keys, big_node_keys) to reduce reallocs queue list
// but average number of keys per node = 1.5 in random fill (97% space hit ratio)
// so node_keys > 2 is generally worse
// Hash32 must return 32-bit value to reduce integer division cost
template<typename T, typename Hash32, uint32_t node_keys = 2, uint32_t big_node_keys = 4>
class fast_hashsetimpl
{
protected:
    struct KeyFlags
    {
        T key       : sizeof(T) * CHAR_BIT - 2;
        T is_offs   : 1;
        T occupied  : 1;
    };
private:
    static_assert( std::is_unsigned<T>::value );
    static_assert( node_keys > 1 );
    static_assert( node_keys < big_node_keys );

    struct big_node
    {
        big_node( T key0, T key1 )
        {
            keys[0].key = key0;
            keys[0].is_offs = 0;
            keys[0].occupied = 1;
            keys[1].key = key1;
            keys[1].is_offs = 0;
            keys[1].occupied = 1;
            for( uint32_t i = 2; i < big_node_keys; ++i )
            {
                keys[i].is_offs = 0;
                keys[i].occupied = 0;
            }
        }
        KeyFlags keys[big_node_keys];
    };

    struct node
    {
        KeyFlags keys[node_keys]; // holds either up two values or value and offset to overdraw table
    };

public:
    fast_hashsetimpl( size_t max_set )
        : ha( static_cast<node*>(calloc(max_set, sizeof(node))), free )
        , size_(max_set), count_(0)
        {
            olist.reserve( max_set / 100 );
        }

    bool insert( T key )
    {
        uint32_t h32 = Hash32{}(key) % size_;
        node &n = ha[h32];
        // try to find free cell on first level table
        for( uint32_t i = 0; i < node_keys; ++i )
        {
            if( 0 == n.keys[i].occupied )
            {
                // simple case cell is free
                new_key(key, n.keys[i]);
                return true;
            }
            if( 0 == n.keys[i].is_offs && key == n.keys[i].key )
                return false;
        }
        // cell is full
        // traverse deep into big nodes ?
        if( n.keys[node_keys - 1].is_offs )
            return traverse(key, olist[n.keys[node_keys - 1].key]);

        allocate_big(key, n.keys[node_keys - 1]);
        return true;
    }

    bool find( T key ) const noexcept
    {
        uint32_t h32 = Hash32{}(key) % size_;
        node &n = ha[h32];
        // try to find free cell on first level table
        for( uint32_t i = 0; i < node_keys; ++i )
        {
            if( 1 == n.keys[i].occupied )
            {
                if( 0 == n.keys[i].is_offs )
                {
                    if( key == n.keys[i].key )
                        return true; // found on first level
                }
                else
                    return traverse_find(key, olist[n.keys[i].key]);
            }
        }

        return false;
    }

    void clear()
    {
        count_ = 0;
        memset(ha.get(), 0, size_ * sizeof(node));
        olist.clear();
    }

    size_t count() const { return count_; }

    size_t capacity() const { return size_; }

    size_t overdrawn_size() const { return olist.size(); }
private:
    void new_key( T key, KeyFlags &kn )
    {
        kn.key = key;
        kn.is_offs = 0;
        kn.occupied = 1;
        ++count_;
    }

    void allocate_big( T key, KeyFlags &last_k )
    {
        // move key from last node
        // to the first key in new big node
        uint32_t key0 = last_k.key;
        uint32_t offs = olist.size();
        olist.emplace_back(key0, key);
        last_k.key = offs;
        last_k.is_offs = 1;
        last_k.occupied = 1;
        ++count_;
    }

    bool traverse_find( T key, big_node const &n ) const noexcept
    {
        for( uint32_t i = 0; i < big_node_keys; ++i )
        {
            if( 1 == n.keys[i].occupied )
            {
                if( 0 == n.keys[i].is_offs )
                {
                    if( key == n.keys[i].key )
                        return true;
                }
                else
                    return traverse_find(key, olist[n.keys[i].key]);
            }
        }
        return false;
    }

    bool traverse( T key, big_node &n )
    {
        for( uint32_t i = 0; i < big_node_keys; ++i )
        {
            if( 0 == n.keys[i].occupied )
            {
                new_key(key, n.keys[i]);
                return true;
            }
            if( 0 == n.keys[i].is_offs && key == n.keys[i].key )
                return false;
        }
        if( n.keys[big_node_keys - 1].is_offs )
            return traverse(key, olist[n.keys[big_node_keys - 1].key]);
        // need to allocate new big node
        allocate_big(key, n.keys[big_node_keys - 1]);
        return true;
    }
private:
    std::unique_ptr<node[], decltype( &free )> ha;
    uint32_t const size_;
    uint32_t count_;
    std::vector<big_node>   olist; // overdrawn list
};


/* special version of hashtable:
   can hold up to 30-bit unsigned integers
   in the special way to handle fast unique insert
   +++ vs std::unordered_set<uint32_t>
   uses lower memory footprint
   faster insert
   faster clean
   ---
   only 30-bits of key is used
   rehashing not supported on the fly
 */
template<uint32_t node_keys = 2, uint32_t big_node_keys = 4>
struct fast_hashset30 : fast_hashsetimpl<uint32_t, utils::hash32, node_keys, big_node_keys>
{
    using B = fast_hashsetimpl<uint32_t, utils::hash32, node_keys, big_node_keys>;
    static_assert( sizeof(typename B::KeyFlags) == 4 );
    using fast_hashsetimpl<uint32_t, utils::hash32, node_keys, big_node_keys>::fast_hashsetimpl;
};

// same as above, but for 64 bit numbers, real used value is 62 bits only
template<uint32_t node_keys = 2, uint32_t big_node_keys = 4>
struct fast_hashset62 : fast_hashsetimpl<uint64_t, utils::hash32, node_keys, big_node_keys>
{
    using B = fast_hashsetimpl<uint64_t, utils::hash32, node_keys, big_node_keys>;
    static_assert( sizeof(typename B::KeyFlags) == 8 );
    using fast_hashsetimpl<uint64_t, utils::hash32, node_keys, big_node_keys>::fast_hashsetimpl;
};


// generic bitmap with fast clear
// for example you have relative large bitmap with very low(<2%) fill
// to reduce clear penalty, this class provides additinal bitmap
// to clear every N(64-512) bytes of main bitmap only if we have hit into it
class bitmap_fclear
{
    static size_t constexpr page_size = 64; // bytes
    static size_t constexpr page_size_words = page_size / sizeof(uint64_t);
    static size_t constexpr page_size_bits = page_size * 8;
    static_assert( page_size >= 64 );
public:
    bitmap_fclear( size_t max_capacity )
        : count_(0)
        , small_capacity_(utils::get_aligned(max_capacity / page_size_bits, 
            64) / sizeof(uint64_t))
        , total_wsz_(utils::get_aligned(max_capacity, 
            page_size_bits) / 64 /*main bitmap*/ + small_capacity_)
        , bitmap_( static_cast<uint64_t*>(utils::aligned_malloc(total_wsz_ * sizeof(uint64_t))) )
        , clear_bm_(bitmap_ + utils::get_aligned(max_capacity, 
            page_size_bits) / 64)
        {
            utils::zero_mem64(bitmap_, total_wsz_ * sizeof(uint64_t));
        }

    ~bitmap_fclear()
    {
        free(bitmap_);
    }

    bool insert( size_t id )
    {
        uint64_t &w = bitmap_[ id / 64 ];
        if( 0 == ((w >> (id % 64)) & 1UL)  )
        {
            touch_bm(w, id);
            if( count_ < small_capacity_ )
                touch_bm(clear_bm_, id / page_size_bits);
            ++count_;
            return true;
        }

        return false;
    }
    size_t count() const { return count_; }

    void clear()
    {
        uint32_t pos;
        uint32_t cnt = count_;
        uint64_t wposes;
        count_ = 0;
        if( cnt < small_capacity_ )
        {
            // go thru small bitmap bits
            for( uint64_t *offs = bitmap_, *it = clear_bm_, *end = it + small_capacity_; 
                it != end; ++it, offs += page_size_words * 64 )
            {
                wposes = *it;
                if( wposes )
                {
                    *it = 0;
                    {
                        // scan word and get bits poses one by one
                        do
                        {
                            pos = __builtin_ffsll(wposes) - 1;
                            // clear bit
                            wposes &= (wposes - 1);
                            // clear page
                            utils::zero_mem64(offs + pos * page_size_words, page_size);
                        }
                        while( wposes > 0 );
                    }
                }
            }
        }
        else
        {
            // bitmap is very dense, so zeroing all
            utils::zero_mem64(bitmap_, total_wsz_ * sizeof(uint64_t));
        }
    }
private:
    void touch_bm( uint64_t &w, size_t addr )
    {
        w |= 1UL << (addr % 64);
    }
    void touch_bm( uint64_t *bm, size_t addr )
    {
        uint64_t &w = bm[ addr / 64 ];
        touch_bm(w, addr);
    }
private:
    uint32_t count_, small_capacity_, total_wsz_;
    uint64_t *bitmap_, *clear_bm_;
};
