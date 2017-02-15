#pragma once

#include "Vector.h"
#include "Heap.h"
#include "Gtl_Common.h"
#include <iostream>

namespace GTL
{
    template <class T, class Compare, class Sequence = Vector<T> >
    class PriorityQueue
    {
    public:
        typedef typename Sequence::value_type           value_type;
        typedef typename Sequence::size_type            size_type;
        typedef typename Sequence::reference            reference;
        typedef typename Sequence::const_reference      const_reference;
        typedef typename Sequence::pointer              pointer;

    public:
        template <class InputIterator>
        PriorityQueue(InputIterator first, InputIterator last, Compare const& comparer)
            : _SequenceInternal(first, last), _Comparer(comparer) 
        {
            MakeHeap(_SequenceInternal.Begin(), _SequenceInternal.End(), _Comparer);
        }

        template <class InputIterator>
        PriorityQueue(InputIterator first, InputIterator last)
            : _SequenceInternal(first, last) 
        {
            MakeHeap(_SequenceInternal.Begin(), _SequenceInternal.End(), _Comparer);
        }

        bool Empty() const
        {
            return _SequenceInternal.Empty();
        }

        size_type Size() const
        {
            return _SequenceInternal.Size();
        }

        reference Top()
        {
            return _SequenceInternal.Front();
        }

        void Push(value_type const &obj)
        {
            _SequenceInternal.PushBack(obj);
            PushHeap(_SequenceInternal.Begin(). _SequenceInternal.End(), _Comparer);
        }

        void Pop()
        {
            PopHeap(_SequenceInternal.Begin(), _SequenceInternal.End(), _Comparer);
            _SequenceInternal.PopBack();
        }

        void Display()
        {
            std::cout << Size() << " :" << std::endl;
            while (!Empty())
            {
                PriorityQueue<T, Compare>::const_reference topElem = Top();
                std::cout << topElem << " ";
                Pop();
            }
            std::cout << std::endl;
        }

    private:
        Sequence _SequenceInternal;
        Compare _Comparer;
    };
}