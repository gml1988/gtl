#pragma once

#include "Iterator_Traits.h"
#include "Type_Traits.h"

namespace GTL
{
    template <class T>
    inline void Construct(T *p)
    {
        new (p) T();
    }

    template <class T1, class T2>
    inline void Construct(T1* p, const T2& value)
    {
        new (p) T1(value);
    }

    template <class T>
    inline void Destroy(T* p)
    {
        p->T::~T();
    }

    template <class ForwardIterator>
    inline void __Destroy_Aux(ForwardIterator first, ForwardIterator last, __false_type)
    {
        for (; first < last; ++first)
        {
            Destory(&*first);
        }
    }

    template <class ForwardIterator>
    inline void __Destroy_Aux(ForwardIterator first, ForwardIterator last, __true_type)
    {
        
    }

    template <class ForwardIterator, class T>
    inline void __Destroy(ForwardIterator first, ForwardIterator last, T*)
    {
        typedef typename type_traits<T>::has_trivial_destructor has_trivial_destructor;
        __Destroy_Aux(first, last, has_trivial_destructor());
    }

    template <class ForwardIterator>
    inline void Destroy(ForwardIterator first, ForwardIterator last)
    {
        __Destroy(first, last, Value_Type(first));
    }

    template <class ForwardIterator>
    inline void Destroy(ForwardIterator iter)
    {
        Destory(&*iter);
    }
}