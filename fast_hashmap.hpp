#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <limits.h> // for CHAR_BIT
#include <type_traits>

namespace utils {

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
