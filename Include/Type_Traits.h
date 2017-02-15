#pragma once

#include "Gtl_Common.h"

namespace GTL
{
    template <class T>
    struct type_traits
    {
        typedef typename T::has_trivial_default_constructor		has_trivial_default_constructor;
        typedef typename T::has_trivial_constructor             has_trivial_constructor;
        typedef typename T::has_trivial_assignment_operator     has_trivial_assignment_operator;
        typedef typename T::has_trivial_destructor              has_trivial_destructor;
        typedef typename T::is_POD_type                         is_POD_type;
    };

    template <>
    struct type_traits<char>
    {
		typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };

	template <>
    struct type_traits<unsigned char>
    {
		typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };

    template <>
    struct type_traits<wchar_t>
    {
        typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };

    template <>
    struct type_traits<int>
    {
        typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };

    template <>
    struct type_traits<unsigned int>
    {
        typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };

    template <class T>
    struct type_traits<T*>
    {
        typedef __true_type						has_trivial_default_constructor;
        typedef __true_type						has_trivial_constructor;
		typedef __true_type						has_trivial_assignment_operator;
        typedef __true_type						has_trivial_destructor;
        typedef __true_type						is_POD_type;
    };
}