#pragma once

#include <iostream>

namespace GTL
{
    template <class T, T v>
    struct Integral_Constant
    {
    static const T value = v;
    typedef T value_type;
    typedef Integral_Constant<T, v> type;
    };

    typedef Integral_Constant<bool, true> true_type;
    typedef Integral_Constant<bool, false> false_type;

    // The structure used for the Iterator Type Tag
    // The inheritance of the tag structure reflect the inheritance of the corresponding iterator
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    typedef true_type __true_type;
    typedef false_type __false_type;

    template <class Container>
    inline void Display(Container &container)
    {
        std::cout << container.Size() << " :" << std::endl;
        for (Container::iterator iter = container.Begin(); iter != container.End(); ++iter)
        {
            std::cout << *iter << " ";
        }
        std::cout << std::endl;
    }

    template <class T>
    inline void Swap(T &a, T &b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    };
}