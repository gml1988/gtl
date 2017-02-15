#pragma once

namespace GTL
{
	template <class T1, class T2>
    struct Pair
    {
    public:
        typedef T1 first_type;
        typedef T2 second_type;

    public:
        Pair() : first(T1()), second(T2()) {};
        Pair(T1 const &a, T2 const &b) : first(a), second(b) {};

    public:
        T1 first;
        T2 second;
    };
}