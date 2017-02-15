#pragma once

#include "Alloc.h"
#include "Gtl_Common.h"
#include "Gtl_Construct.h"
#include "Gtl_Uninitialized.h"
#include <crtdefs.h>
#include <memory>

namespace GTL
{
    template <class T, class Alloc=Allocator>
    class Vector
    {
        //Need define the typename for the internal use, abstract the expression of the type
    public:
        typedef T                    value_type;
        typedef T&                    reference;
        typedef T const&            const_reference;
        typedef T*                    pointer;
        typedef T*                    iterator;
        typedef T const*            const_iterator;
        typedef size_t                size_type;
        typedef ptrdiff_t            difference_type;
        typedef Vector<T, Alloc>    self;

    public:
        Vector() : _iterStart(nullptr), _iterFinish(nullptr), _iterEndOfStorage(nullptr)
        {
            
        }

        Vector(size_type nSize, T const &value)
        {
            _FillAndInitialize(nSize, value);
        }

        template <class Iterator>
        Vector(Iterator first, Iterator last, typename std::enable_if<std::_Is_iterator<Iterator>::value, void>::type ** = 0)
            : _iterStart(nullptr), _iterFinish(nullptr), _iterEndOfStorage(nullptr)
        {
            Reserve(last - first + 1);
            Insert(_iterFinish, first, last);
        }

        ~Vector()
        {
            Destroy(_iterStart, _iterFinish);
            Deallocate();
        }

        size_type Size() const
        {
            return _iterFinish - _iterStart;
        }

        size_type Capacity() const
        {
            return _iterEndOfStorage - _iterStart;
        }

        bool Empty() const
        {
            return _iterStart == _iterFinish;
        }

        iterator Begin() const
        {
            return _iterStart;
        }

        iterator End() const
        {
            return _iterFinish;
        }

        reference operator[](size_type pos)
        {
            return *(_iterStart + pos);
        }

        const_reference operator[](size_type nPos) const
        {
            return *(_iterStart + nPos);
        }

        reference Front()
        {
            return *_iterStart;
        }

        reference Back()
        {
            return *_iterFinish;
        }

        void PushBack(T const &value)
        {
            if (_iterFinish != _iterEndOfStorage)
            {
                Construct(_iterFinish, value);
                ++_iterFinish;
            }
            else
            {
                _Insert_Aux(_iterFinish, value);
            }
        }

        void PopBack()
        {
            if (_iterFinish != _iterStart)
            {
                --_iterFinish;
                Destroy(_iterFinish);
            }
        }

        void Insert(iterator &iter, T const &value)
        {
            _Insert_Aux(iter, value);
        }

        template <class Iterator>
        void Insert(iterator iter, Iterator first, Iterator last, typename std::enable_if<std::_Is_iterator<Iterator>::value, void>::type ** = 0)
        {
            while (first < last)
            {
                _Insert_Aux(iter, *first);
                ++iter;
                ++first;
            }
        }

        void Insert(iterator &iter, size_type nSize, T const &value)
        {
            // Temp solution - TBD
            while (nSize > 0)
            {
                _Insert_Aux(iter, value);
                --nSize;
            }
        }

        iterator Erase(iterator iter)
        {
            if (iter + 1 != _iterFinish)
            {
                std::copy(iter + 1, _iterFinish, iter);
            }
            Destroy(iter);
            return iter;
        }

        // Erase the element in the range [first, last)
        iterator Erase(iterator first, iterator last)
        {
            if (last != _iterFinish)
            {
                std::copy(last, _iterFinish, first);
                iterator iterNewFinish = first + (_iterFinish - last);
                Destroy(iterNewFinish, _iterFinish);
                _iterFinish = iterNewFinish;
            }

            return first;
        }

        void Resize(size_type nNewSize, T const &value)
        {
            size_type nCurrentSize = Size();
            if (nNewSize < nCurrentSize)
            {
                Erase(_iterStart + nNewSize, _iterFinish);
            }
            else if(nNewSize > nCurrentSize)
            {
                Insert(_iterFinish, nNewSize - nCurrentSize, value);
            }
        }

        void Reserve(size_type nNewCapacity)
        {
            size_type nCurrentCapacity = Capacity();
            // If current capacity can't meet the request, allocate a new memory and copy the previous content to the new space
            if (nCurrentCapacity < nNewCapacity)
            {
                T *pNewArray = DataAllocator::Allocate(nNewCapacity);
                iterator iterNewStart(pNewArray);
                std::copy(_iterStart, _iterFinish, iterNewStart);
                iterator iterNewFinish = iterNewStart + Size();
                Deallocate();
                _iterStart = iterNewStart;
                _iterFinish = iterNewFinish;
                _iterEndOfStorage = _iterStart + nNewCapacity;
            }
        }

        void Clear()
        {
            Destroy(_iterStart, _iterFinish);
            _iterFinish = _iterStart;
        }

        void Swap(self &other)
        {
            GTL::Swap<iterator>(_iterStart, other._iterStart);
            GTL::Swap<iterator>(_iterFinish, other._iterFinish);
            GTL::Swap<iterator>(_iterEndOfStorage, other._iterEndOfStorage);
        }

    protected:
        typedef Simple_Allocator<T, Alloc> DataAllocator;

    private:
        void _Insert_Aux(iterator const &iter, T const &value)
        {
            if (_iterFinish != _iterEndOfStorage)
            {
                std::copy_backward(iter, _iterFinish, iter + 1);
                Construct(iter, value);
                ++_iterFinish;
            }
            else
            {
                size_type nCurrentCapacity = Capacity();
                // Double the capacity of the new vector container
                size_type nNewCapacity = (nCurrentCapacity > 0) ? (nCurrentCapacity << 1) : s_cnInitCapacity;
                // Allocate a new memory with larger capacity
                T* pNewArray = DataAllocator::Allocate(nNewCapacity);
                iterator iterNewStart(pNewArray);
                iterator iterNewFinish = iterNewStart + Size() + 1;
                iterator iterNewEndOfStorage = iterNewStart + nNewCapacity;
                // Copy data in the range of [0, n) to new [0, n)
                std::copy(_iterStart, iter, iterNewStart);
                // Copy data in the range of [n, end) to new [n+1, end+1)
                difference_type dInsIterAndStart = iter - _iterStart;
                iterator iterNewInsert = iterNewStart + dInsIterAndStart;
                std::copy(iter, _iterFinish, iterNewStart + 1);
                // Construct the data in the position to be inserted
                Construct(iterNewInsert, value);
                // Destory the data in the previous vector
                Destroy(_iterStart, _iterFinish);
                Deallocate();
                // Assign the iterator in the new vector
                _iterStart = iterNewStart;
                _iterFinish = iterNewFinish;
                _iterEndOfStorage = iterNewEndOfStorage;
            }
        }

        void _FillAndInitialize(size_type nSize, T const &value)
        {
            _iterStart = _AllocateAndFill(nSize, value);
            _iterFinish = _iterStart + nSize;
            _iterEndOfStorage = _iterStart + nSize;
        }

        iterator _AllocateAndFill(size_type nSize, T const &value)
        {
            T* pNewAllocArray = DataAllocator::Allocate(nSize);
            iterator iterNewStart(pNewAllocArray);
            Uninitialized_Fill_N(iterNewStart, nSize, value);
            return iterNewStart;
        }

        // Release the container memory for the vector
        void Deallocate()
        {
            if (_iterStart)
            {
                DataAllocator::Deallocate(_iterStart, _iterEndOfStorage - _iterStart);
            }
        }

    private:
        static const size_type s_cnInitCapacity = 20;
        iterator _iterStart;                    //The buffer between iterStart and iterEndOfStorge is reserved from memory to accomodate the elements
        iterator _iterFinish;
        iterator _iterEndOfStorage;
    };
}