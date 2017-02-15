#pragma once

#include "Gtl_Common.h"
#include "Gtl_Construct.h"
#include "Type_Traits.h"
#include <algorithm>

namespace GTL
{
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __Uninitialized_Copy_Aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
    {
        for (; first < last; ++first, ++result)
        {
            Construct(&*first, result);
        }

        return result;
    }

    template <class InputIterator, class ForwardIterator>
    ForwardIterator __Uninitialized_Copy_Aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
    {
        return std::copy(first, last, result);
    }

    template <class InputIterator, class ForwardIterator, class T>
    ForwardIterator __Uninitialized_Copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
    {
        typedef typename type_traits<T>::is_POD_type is_POD_type;
        return __Uninitialized_Copy_Aux(first, last, result, is_POD_type());
    }

    template <class InputIterator, class ForwardIterator>
    ForwardIterator Uninitialized_Copy(InputIterator first, InputIterator last, ForwardIterator result)
    {
        return Uninitialized_Copy(first, last, result, Value_Type(first));
    }

    template <class ForwardIterator, class T>
    void __Uninitialized_Fill_Aux(ForwardIterator first, ForwardIterator last, const T& value, __false_type)
    {
        std::fill(first, last, value);
    }

    template <class ForwardIterator, class T>
    void __Uninitialized_Fill_Aux(ForwardIterator first, ForwardIterator last, const T& value, __true_type)
    {
        for (; first != last; ++first)
        {
            Construct(&*first, value); 
        }
    }

    template <class ForwardIterator, class T, class T1>
    void __Uninitialized_Fill(ForwardIterator first, ForwardIterator last, const T& value, T1*)
    {
        typedef typename type_traits<T1>::is_POD_type is_POD_type;
        __Uninitialized_Fill_Aux(first, last, value, is_POD_type());
    }

    template <class ForwardIterator, class T>
    void Uninitialized_Fill(ForwardIterator first, ForwardIterator last, const T& value)
    {
        __Uninitialized_Fill(first, last, value, Value_Type(first));
    }

    template <class ForwardIterator, class Size, class T>
    ForwardIterator __Uninitialize_Fill_N_Aux(ForwardIterator first, Size n, const T& value, __false_type)
    {
        return std::fill_n(first, n, value);
    }

    template <class ForwardIterator, class Size, class T>
    ForwardIterator __Uninitialize_Fill_N_Aux(ForwardIterator first, Size n, const T& value, __true_type)
    {
        for (Size index = 0; index < n; ++index, ++first)
        {
            Construct(&*first, value);
        }

        return first;
    }

    template <class ForwardIterator, class Size, class T, class T1>
    ForwardIterator __Uninitialize_Fill_N(ForwardIterator first, Size n, const T& value, T1*)
    {
        typedef typename type_traits<T1>::is_POD_type is_POD_type;
        return __Uninitialize_Fill_N_Aux(first, n, value, is_POD_type());
    }

    template <class ForwardIterator, class Size, class T>
    ForwardIterator Uninitialized_Fill_N(ForwardIterator first, Size n, const T& value)
    {
        return __Uninitialize_Fill_N(first, n, value, Value_Type(first));
    }
}