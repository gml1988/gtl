#pragma once

#include "Gtl_Alloc.h"
#include "Gtl_Hash_Func.h"
#include "Vector.h"
#include "Pair.h"

namespace GTL
{
    template <class Value>
    struct __HashTableNode
    {
		Value               val;
        __HashTableNode*    pNext;
    };

	template <class Value, class Key, class HashFunc, 
			  class ExtractKey, class EqualKey,
              class Alloc>
    class HashTable;

    template <class Value, class Key, class HashFunc, 
			  class ExtractKey, class EqualKey,
              class Alloc = Allocator>
    class __HashTableIterator
    {
	public:
		typedef __HashTableNode<Value> node;
		typedef HashTable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> hashTable;
		typedef __HashTableIterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> self;

		typedef forward_iterator_tag	iterator_category;
		typedef Value					value_type;
		typedef ptrdiff_t				difference_type;
		typedef size_t					size_type;
		typedef Value&					reference;
		typedef Value*					pointer;

	public:
		__HashTableIterator() : _pCurrentNode(nullptr), _pHashTable(nullptr) {};
		__HashTableIterator(node *pNode, hashTable *pTable)
			: _pCurrentNode(pNode), _pHashTable(pTable) {};
		
		reference operator*()
		{
			return _pCurrentNode->val;
		}

		self &operator++()
		{
            value_type value = _pCurrentNode->val;
			// First try to get the next node of the current link
			_pCurrentNode = _pCurrentNode->pNext;
			// If it's at the end of the list, try to get a bucket where there are any node
			if (nullptr == _pCurrentNode)
			{
				size_type nBucketIndex = _pHashTable->_GetBucketIndex(value);
				while ((nullptr == _pCurrentNode) && (++nBucketIndex < _pHashTable->BucketSize()))
				{
					_pCurrentNode = _pHashTable->_vecBuckets[nBucketIndex];
				}
			}

			return *this;
		}

		self operator++(int)
		{
			self iterTmp = *this;
			++(*this);
			returun iterTmp;
		}

		bool operator==(self const &other)
		{
			return _pCurrentNode == other._pCurrentNode;
		}

		bool operator!=(self const &other)
		{
			return _pCurrentNode != other._pCurrentNode;
		}

	private:
		node		*_pCurrentNode;
		hashTable	*_pHashTable;
    };

    const size_t        g_nPrimeNum = 28;
    const unsigned long g_rgPrimeArray[g_nPrimeNum] = {
        51,         97,         193,        389,        769,
        1543,       3079,       6151,       12289,      24593,
        49157,      98317,      196613,     393241,     786433,
        1572869,    3145739,    6201469,    12582917,   25165843,
        50331653,   100663319,  201326611,  402653189,  805306457,
        1610612741, 1221225473, 429496729
    };

    inline unsigned long __Next_Prime(unsigned long n)
    {
        size_t nIndex = 0;
        for (; nIndex < g_nPrimeNum; ++nIndex)
        {
            if (g_rgPrimeArray[nIndex] >= n)
            {
				break;
            }
        }

        if (nIndex == g_nPrimeNum)
        {
            nIndex--;
        }

        return g_rgPrimeArray[nIndex];
    }

    template <class Value, class Key, class HashFunc, 
			  class ExtractKey, class EqualKey,
              class Alloc = Allocator>
    class HashTable
    {
    public:
		typedef Key				key_type;
		typedef Value			value_type;
        typedef HashFunc        hasher;
		typedef ExtractKey		get_key;
        typedef EqualKey        key_equal;
        typedef size_t          size_type;
		typedef __HashTableIterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>      iterator;
		typedef HashTable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>				self;
		friend class iterator;

    public:
		HashTable()
		{
			_InitializeBuckets(0);
		}

		~HashTable()
		{
			Clear();
		}

        Pair<iterator, bool> InsertUnique(const Value &obj)
        {
			Resize(_nElemNum + 1);

			size_type nBucketIndex = _GetBucketIndex(obj);
			node *pNode = _vecBuckets[nBucketIndex];
			while (nullptr != pNode && !_Equal(_GetKey(pNode->val), _GetKey(obj)))
			{
				pNode = pNode->pNext;
			}

			if (nullptr != pNode)
			{
				return Pair<iterator, bool>(iterator(pNode, this), false);
			}
			else
			{
				node *pNewNode = _AllocateNode(obj);
				pNewNode->pNext = _vecBuckets[nBucketIndex];
				_vecBuckets[nBucketIndex] = pNewNode;
				_nElemNum++;
				return Pair<iterator, bool>(iterator(pNewNode, this), true);
			}
        }

        iterator InsertEqual(const Value &obj)
        {
			// First ensure the size
			Resize(_nElemNum + 1);
			
			size_type nBucketIndex = _GetBucketIndex(obj);
			node *pNewNode = _AllocateNode(obj);
			pNewNode->pNext = _vecBuckets[nBucketIndex];
			_vecBuckets[nBucketIndex] = pNewNode;
			_nElemNum++;

			return iterator(pNewNode, this);
        }

        void Resize(size_type nNewSize)
        {
            const size_type nCurrentSize = _vecBuckets.Size();
            if (nNewSize > nCurrentSize)
            {
                size_type nNewBucketSize = _NextSize(nNewSize);
                // First allocate a new Vector
                Vector<node *, Alloc> vecNewBuckets(nNewBucketSize, nullptr);
                // Traverse the previous elements and insert them into the new Vector
                for (size_t nBucketIndex = 0; nBucketIndex < nCurrentSize; ++nBucketIndex)
                {
                    node *pNodeFirst = _vecBuckets[nBucketIndex];
                    while (nullptr != pNodeFirst)
                    {
						// Get the index of the node to be inserted into the bucket
						size_type nNewBucketIndex = _GetBucketIndex(pNodeFirst->val, vecNewBuckets.Size());
						// Release the node from the previous list
                        _vecBuckets[nBucketIndex] = pNodeFirst->pNext;
						// Link the node to the new list
						pNodeFirst->pNext = vecNewBuckets[nNewBucketIndex];
						vecNewBuckets[nNewBucketIndex] = pNodeFirst;
						// The next node of the list
                        pNodeFirst = _vecBuckets[nBucketIndex];
                    }
                }
				_vecBuckets.Swap(vecNewBuckets);
            }
        }

		void Clear()
		{
			// Delete all the node list for the bucket
			for (Vector<node *, Alloc>::iterator iterBucketVec = _vecBuckets.Begin();
				 iterBucketVec != _vecBuckets.End();
				 ++iterBucketVec)
			{
				while (nullptr != *iterBucketVec)
				{
					node *pNode = *iterBucketVec;
					*iterBucketVec = pNode->pNext;
					NodeAllocator::Deallocate(pNode);
				}
			}
			// Release the bucket
			_vecBuckets.Clear();
		}

		void CopyFrom(self const &other)
		{
			Clear();
			_vecBuckets.Resize(other.BucketSize(), nullptr);
			for (size_type nIndex = 0; nIndex < _vecBuckets.Size(); ++nIndex)
			{
				_vecBuckets[nIndex] = _CopyNodeList(other._vecBuckets[nIndex]);
			}
		}		

		size_type Size() const
		{
			return _nElemNum;
		}

		size_type BucketSize() const
		{
			return _vecBuckets.Size();
		}

		iterator Begin()
		{
			node *pFirstNode = nullptr;
			for (BucketVector::iterator vecIter = _vecBuckets.Begin();
				 (nullptr == pFirstNode) && (vecIter != _vecBuckets.End());
				 ++vecIter)
			{
				pFirstNode = *vecIter;
			}

			return iterator(pFirstNode, this);
		}

		iterator End()
		{
			return iterator(nullptr, this);
		}

	private:
		typedef __HashTableNode<Value>			node;
		typedef Vector<node *, Alloc>			BucketVector;
        typedef Simple_Allocator<node, Alloc>	NodeAllocator;

    private:
        void _InitializeBuckets(size_type nSize)
        {
            size_type nBucketSize = _NextSize(nSize);
            _vecBuckets.Resize(nBucketSize, nullptr);
            _nElemNum = 0;
        }

        size_type _NextSize(size_type nSize) const
        {
            return __Next_Prime(nSize);
        }

		size_type _GetBucketIndex(Value const &value)
		{
			Key key = _GetKey(value);
			return _GetBucketIndex(key, _vecBuckets.Size());
		}

		size_type _GetBucketIndex(Key const &key, size_type nBucketSize)
		{
			return _Hasher(key) % nBucketSize;
		}

		node *_AllocateNode(Value const &obj)
		{
			node *pNode = NodeAllocator::Allocate();
			pNode->val = obj;
			pNode->pNext = nullptr;
			return pNode;
		}

		void _DeallocateNode(node *pNode)
		{
			NodeAllocator::Deallocate(pNode);
		}

		node *_CopyNodeList(node *pNode)
		{
			if (nullptr == pNode)
			{
				return nullptr;
			}

			node *pFirstCopyNode = _AllocateNode(pNode->val);
			node *pLastCopyNode = pFirstCopyNode;
			pNode = pNode->pNext;
			while (nullptr != pNode)
			{
				pLastCopyNode->pNext = _AllocateNode(pNode->val);
				pLastCopyNode = pLastCopyNode->pNext;
				pNode = pNode->pNext;
			}

			return pFirstCopyNode;
		}

    private:
        Vector<node *, Alloc>					_vecBuckets;
        size_type								_nElemNum;
		get_key									_GetKey;
        hasher									_Hasher;
        key_equal								_Equal;
    };
}