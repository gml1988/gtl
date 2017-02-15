#pragma once

#include "Gtl_Hash_Func.h"
#include "HashTable.h"

namespace GTL
{
    // Adapter, internal class is HashTable
    template <class Value, class HashFunc, class EqualKey, class Alloc>
    class HashSet
    {
    public:
        typedef typename hashTable::value_type                value_type;
        typedef typename hashTable::key_type                key_type;
        typedef typename hashTable::iterator                iterator;
        typedef HashSet<Value, HashFunc, EqualKey, Alloc>   self;

    public:
        Pair<iterator, bool> Insert(value_type const &obj)
        {
            Pair<iterator, bool> p = _hashTableInternal.InsertUnique(obj);
            return p;
        }

    private:
        typedef HashTable<Value, Value, HashFunc, GetKey<Value>, EqualKey, Alloc> hashTable;
        
    private:
        hashTable _hashTableInternal;
    };
}