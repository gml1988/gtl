#pragma once

#include "Alloc.h"
#include "Gtl_Common.h"
#include "Gtl_Uninitialized.h"
#include "Numeric.h"
#include <exception>

namespace GTL
{
    inline size_t __Deque_Buf_Size(size_t nUserDefSize, size_t nElemSize)
    {
        return (nUserDefSize != 0 ? nUserDefSize : (nElemSize < 512 ? (size_t)(512 / nElemSize) : size_t(1)));        
    }

    template <class T, class Ref, class Ptr, size_t BufSize>
    struct __Deque_Iterator
    {
    public:
        typedef __Deque_Iterator<T, T&, T*, BufSize> iterator;
        typedef __Deque_Iterator<T, T const &, T const *, BufSize> const_iterator;
        typedef T                   value_type;
        typedef Ref                 reference;
        typedef Ptr                 pointer;
        typedef size_t              size_type;
        typedef ptrdiff_t           difference_type;
        typedef T**                 MapPointer;
        typedef __Deque_Iterator    self;
    
    public:
        static size_type BufferSize()
        {
            return __Deque_Buf_Size(BufSize, sizeof(T));
        }

        reference operator*() const
        {
            return *pCurrent;
        }

        pointer operator->() const
        {
            return pCurrent;
        }

        difference_type operator-(const self& other) const
        {
            return difference_type(BufferSize()) * (mpLocateNode - other.mpLocateNode - 1) + (pCurrent - pFirst) + (other.pLast - other.pCurrent);
        }

        self& operator++()
        {
            ++pCurrent;
            if (pCurrent == pLast)
            {
                SetNode(mpLocateNode + 1);
                pCurrent = pFirst;
            }
            return *this;
        }

        self operator++(int)
        {
            iterator iterTmp = *this;
            ++(*this);
            return iterTmp;
        }

        self& operator--()
        {
            if (pCurrent == pFirst)
            {
                SetNode(mpLocateNode - 1);
                pCurrent = pLast;
            }
            --pCurrent;
            return *this;
        }

        self operator--(int)
        {
            self iterTmp = *this;
            --(*this);
            return iterTmp;
        }

        self& operator+=(difference_type dOffset)
        {
            // If the target position is at the same buffer with the current cursor
            if (pCurrent + dOffset < pLast &&
                pCurrent + dOffset >= pFirst)
            {
                pCurrent += dOffset;
            }
            else
            {
                difference_type nCurrentPos = pCurrent - pFirst;
                difference_type nTargetPos = nCurrentPos + dOffset;
                difference_type dNodeOffset = (dOffset >= 0) ? (nTargetPos / BufferSize()) : (-((-nTargetPos) / BufferSize()));
                difference_type dTargetOffsetInBuf = (dOffset >= 0) ? (nTargetPos % BufferSize()) : (BufSize() + 1 - (-nTargetPos) % BufferSize());
                SetNode(mpLocateNode + dNodeOffset);
                pCurrent = pFirst + dTargetOffsetInBuf;
            }
        }

        self operator+(difference_type dOffset)
        {
            self iterTmp = *this;
            iterTmp += dOffset;
            return iterTmp;
        }

        self* operator-=(difference_type dOffset)
        {
            return *this += (-dOffset);
        }

        self operator-(difference_type dOffset)
        {
            self iterTmp = *this;
            iterTmp -= dOffset;
            return iterTmp;
        }

        reference operator[](difference_type dPos)
        {
            return *(*this + dPos);
        }

        bool operator==(const self &other) const
        {
            return (mpLocateNode == other.mpLocateNode) && (pFirst == other.pFirst) && (pLast == other.pLast) && (pCurrent == other.pCurrent);
        }

        bool operator!=(const self &other) const
        {
            return !operator==(other);
        }

        void SetNode(MapPointer mpNode)
        {
            mpLocateNode = mpNode;
            pFirst = *mpNode;
            pLast = pFirst + BufferSize();
        }

        void Reset()
        {
            mpLocateNode = nullptr;
            pFirst = nullptr;
            pLast = nullptr;
            pCurrent = nullptr;
        }

        template <class VRef, class VPtr>
        self operator=(const __Deque_Iterator<T, VRef, VPtr, BufSize> &other)
        {
            pCurrent = other.pCurrent;
            pFirst = other.pFirst;
            pLast = other.pLast;
            mpLocateNode = other.mpLocateNode;
        }

    public:
        T *pCurrent;
        T *pFirst;
        T *pLast;                       // [pFirst, pLast)
        MapPointer mpLocateNode;        // Locate which node the iterator belongs to 
    };

    template <class T, class Alloc = Allocator, size_t BufSize = 0>
    class Deque
    {
    public:
        typedef T                                       value_type;
        typedef T&                                      reference;
        typedef T*                                      pointer;
        typedef __Deque_Iterator<T, T&, T*, BufSize>    iterator;
        typedef typename __Deque_Iterator<T, T&, T*, BufSize>::const_iterator const_iterator;
        typedef size_t                                  size_type;
        typedef ptrdiff_t                               difference_type;
		typedef Deque<T, Alloc>							self;

    public:
        Deque(size_type nSize, const T &value)
            : _iterStart(), _iterFinish(), _Map(nullptr), _nMapSize(0)
        {
            _FillAndInitialize(nSize, value);
        }

        ~Deque()
        {
            _Destroy();
        }

        iterator Begin()
        {
            return _iterStart;
        }

        iterator End()
        {
            return _iterFinish;
        }

        reference Front()
        {
            return *_iterStart;
        }

        reference Back()
        {
            iterator iterTmp = _iterFinish;
            --iterTmp;
            return *iterTmp;
        }

        size_type Size() const
        {
            return _iterFinish - _iterStart;
        }

        bool Empty() const
        {
            return _iterStart == _iterFinish;
        }

        void PushBack(const T &value)
        {
            // Go to the end of the buffer
            if (_iterFinish.pCurrent == _iterFinish.pLast)
            {
                _PushBack_Aux(value);
            }
            else
            {
                Construct(_iterFinish.pCurrent, value);
                _iterFinish.pCurrent++;
            }
        }

        void PushFront(const T &value)
        {
            if (_iterStart.pCurrent == _iterStart.pFirst)
            {
                _PushFront_Aux(value);
            }
            else
            {
                _iterStart.pCurrent--;
                Construct(_iterStart.pCurrent, value);
            }
        }

        // After pop back, if the last node has no element, we won't delete the last buffer at once
        // We will wait until the next pop back to delete it
        void PopBack()
        {
            if (_iterFinish.pCurrent == _iterFinish.pFirst)
            {
                _PopBack_Aux();
            }
            else
            {
                Destory(_iterFinish.pCurrent);
                _iterFinish.pCurrent--;
            }
        }

        // If there is only 1 element, after destory, will delelte the buffer block at once
        void PopFront()
        {
            if (_iterStart.pCurrent + 1 == _iterStart.pLast)
            {
                _PopFront_Aux();
            }
            else
            {
                Destory(_iterStart.pCurrent);
                _iterStart.pCurrent++;
            }
        }

        void Clear()
        {
            
        }

    protected:
        typedef typename Simple_Allocator<value_type, Alloc> DataAllocator;
        typedef typename Simple_Allocator<pointer, Alloc> MapAllocator;
        typedef pointer* MapPointer;

    private:
        void _PushBack_Aux(const T &value)
        {
            _ReserveMapAtBack();
            *(_iterFinish.mpLocateNode + 1) = _AllocateNode();
            _iterFinish.SetNode(_iterFinish.mpLocateNode + 1);
            Construct(_iterFinish.pFirst, value);
            _iterFinish.pCurrent = _iterFinish.pFirst;
        }

        void _PushFront_Aux(const T &value)
        {
            _ReserveMapAtFront();
            *(_iterStart.mpLocateNode - 1) = _AllocateNode();
            _iterStart.SetNode(_iterStart.mpLocateNode - 1);
            Construct(_iterStart.pLast - 1, value);
            _iterStart.pCurrent = _iterStart.pLast - 1;
        }

        void _PopBack_Aux()
        {
            _DeallocateNode(_iterFinish.pFirst);
            _iterFinish.SetNode(_iterFinish.mpLocateNode - 1);
            _iterFinish.pCurrent = _iterFinish.pLast - 1;
            Destory(_iterFinish.pCurrent);
        }

        void _PopFront_Aux()
        {
            Destory(_iterStart.pCurrent);
            _DeallocateNode(_iterStart.pFirst);
            _iterStart.SetNode(_iterStart.mpLocateNode + 1);
            _iterStart.pCurrent = _iterStart.pFirst;
        }

        void _FillAndInitialize(size_type nSize, const value_type &value)
        {
            _CreateMapAndNodes(nSize);
            // Fill the Buffer with the value
            MapPointer mpTraverse = _iterStart.mpLocateNode;
            for (; mpTraverse != _iterFinish.mpLocateNode; ++mpTraverse)
            {
                Uninitialized_Fill(*mpTraverse, *mpTraverse + s_BufferSize(), value);
            }
            // Fill the last node. Part of the buffer need be filled
            Uninitialized_Fill(_iterFinish.pFirst, _iterFinish.pLast, value);
        }

        void _Destroy()
        {
            for (MapPointer mpTraverse = _iterStart.mpLocateNode; mpTraverse != _iterFinish.mpLocateNode; ++mpTraverse)
            {
                Destroy(*mpTraverse, *mpTraverse + s_BufferSize());
                _DeallocateNode(*mpTraverse);
            }
            _iterStart.Reset();
            _iterFinish.Reset();
        }

        void _CreateMapAndNodes(size_type nElemNum)
        {
            size_type nBufSize = s_BufferSize();
            size_type nNodeNum = nElemNum / nBufSize + 1;
            _nMapSize = Max(s_InitialMapSize(), nNodeNum + 2);
            _Map = MapAllocator::Allocate(_nMapSize);
            MapPointer mpStart = _Map + (_nMapSize - nNodeNum) / 2;
            MapPointer mpFinish = mpStart + nNodeNum - 1;

            try
            {
                for (MapPointer mpCurrent = mpStart; mpCurrent <= mpFinish; ++mpCurrent)
                {
                    *mpCurrent = DataAllocator::Allocate(nBufSize);
                }
            }
            catch(std::exception e)
            {
                // Release all the memory which has been already allocated
            }

            _iterStart.SetNode(mpStart);
            _iterStart.pCurrent = *mpStart;
            _iterFinish.SetNode(mpFinish);
            _iterFinish.pCurrent = *mpFinish + nElemNum % nBufSize;
        }

        void _ReserveMapAtBack(size_type nNodeNumToAdd = 1)
        {
            // If the remaining node at the back end is not enough
            if (nNodeNumToAdd > _nMapSize - (_iterFinish.mpLocateNode - _Map + 1))
            {
                _ReallocateMap(nNodeNumToAdd, false);
            }
        }

        void _ReserveMapAtFront(size_type nNodeNumToAdd = 1)
        {
            // If the remaining node at the front end is not enough
            if (difference_type(nNodeNumToAdd) > _iterStart.mpLocateNode - _Map)
            {
                _ReallocateMap(nNodeNumToAdd, true);
            }
        }

        // Just reserve the node in the map, adjust the location or reallocate the map the accomodate the node pointer
        void _ReallocateMap(size_type nNodeNumToAdd, bool fIsAtFront)
        {
            size_type nCurrentNodeNum = _iterFinish.mpLocateNode - _iterStart.mpLocateNode + 1;
            size_type nNewNodeNum = nCurrentNodeNum + nNodeNumToAdd;
            MapPointer mpNewStart = nullptr;

            // Just adjust the location of the node pointer
            // No need reallocate the map, map size is not changed
            if (_nMapSize >= (nNewNodeNum << 1))
            {
                mpNewStart = _Map + ((_nMapSize - nNewNodeNum) >> 1) + (fIsAtFront ? nNodeNumToAdd : 0);
                if (mpNewStart < _iterStart.mpLocateNode)
                {
                    std::copy(_iterStart.mpLocateNode, _iterFinish.mpLocateNode + 1, mpNewStart);
                }
                else
                {
                    std::copy_backward(_iterStart.mpLocateNode, _iterFinish.mpLocateNode + 1, mpNewStart);
                }
            }
            else
            {
                // Reallocate the map, change the map size
                size_type nNewMapSize = _nMapSize + Max(_nMapSize, nNodeNumToAdd) + 2;
                MapPointer mpNewMap = MapAllocator::Allocate(nNewMapSize);
                mpNewStart = mpNewMap + ((nNewMapSize - nNewNodeNum) >> 1) + (fIsAtFront ? nNodeNumToAdd : 0);
                std::copy(_iterStart.mpLocateNode, _iterFinish.mpLocateNode + 1, mpNewStart);
                MapAllocator::Deallocate(_Map, _nMapSize);
                _Map = mpNewMap;
                _nMapSize = nNewMapSize;
            }

            _iterStart.SetNode(mpNewStart);
            _iterFinish.SetNode(mpNewStart + nCurrentNodeNum - 1);
        }

        T *_AllocateNode()
        {
            return DataAllocator::Allocate(s_BufferSize());
        }

        void _DeallocateNode(pointer pBuf)
        {
            return DataAllocator::Deallocate(pBuf, s_BufferSize());
        }

        static size_t s_BufferSize()
        {
            return iterator::BufferSize();
        }

        static size_t s_InitialMapSize()
        {
            return 8;
        }

    private:
        MapPointer          _Map;
        size_type           _nMapSize;
        iterator            _iterStart;
        iterator            _iterFinish;
    };
}