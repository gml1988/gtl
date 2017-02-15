#pragma once

#include "Gtl_Common.h"
#include "Iterator.h"

namespace GTL
{
    template <class Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category        iterator_category;
    };

    // Partially specification template for Iterator_Traits
    template <class T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag                iterator_category;
        typedef T                                        value_type;
        typedef ptrdiff_t                                difference_type;
        typedef T*                                        pointer;
        typedef T&                                        reference;
    };

    template <class T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag                iterator_category;
        typedef T                                        value_type;
        typedef ptrdiff_t                                difference_type;
        typedef const T*                                pointer;
        typedef const T&                                reference;
    };

    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
        Iterator_Category(const Iterator&)
    {
        return iterator_traits<Iterator>::iterator_category();
    }

    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
        Value_Type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }
}