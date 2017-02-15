#pragma once

#include "Alloc.h"
#include "RBTree.h"
#include <map>

namespace GTL
{
    template <class Key, class Value, 
              class ExtractKey, class Compare, 
              class Alloc = Allocator>
    class Map
    {
    public:
        typedef Key            key_type;
        typedef Value        value_type;
        typedef ExtractKey    get_key;
        typedef Compare        key_compare;
        typedef size_t        size_type;

    public:
        size_type Size() const
        {
            return _RBTreeInternel.Size();
        }

    private:
        RBTree<Key, Value, ExtractKey, Compare, Alloc>    _RBTreeInternel;
    };
}