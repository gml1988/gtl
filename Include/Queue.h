#pragma once

#include "Deque.h"

namespace GTL
{
    template <class T, class Sequence = Deque<T> >
    class Queue
    {
    public:
        typedef Sequence::value_type        value_type;
        typedef Sequence::size_type         size_type;
        typedef Sequence::reference         reference;
        typedef Sequence::const_reference   const_reference;
        typedef Queue<T, Sequence>            self;

    public:
        bool Empty() const
        {
            return _SequenceInternal.Empty();
        }

        bool Size() const
        {
            return _SequenceInternal.Size();
        }

        reference Front()
        {
            return _SequenceInternal.Front();
        }

        const_reference Front() const
        {
            return _SequenceInternal.Front();
        }

        reference Back()
        {
            return _SequenceInternal.Back();
        }

        const_reference Back() const
        {
            return _SequenceInternal.Back();
        }

        void Push(value_type const &value)
        {
            _SequenceInternal.PushBack(value);
        }

        void Pop()
        {
            _SequenceInternal.PopFront();
        }

    protected:
        Sequence _SequenceInternal;
    };
}