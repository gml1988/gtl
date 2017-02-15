#pragma once

#include <string>

namespace GTL
{
    template <class T>
    struct Hash 
    {
        size_t operator()(T key) const
        {
            return 0;
        }
    };

    inline size_t __Hash_String(char const *pszStr)
    {
        size_t nHashValue = 0;
        while (*pszStr)
        {
            nHashValue = 5 * nHashValue + (*pszStr);
        }

        return nHashValue;
    }

    template <>
    struct Hash<char *>
    {
        size_t operator()(char *s) const
        {
            return __Hash_String(s);
        }
    };

    template <>
    struct Hash<char const *>
    {
        size_t operator()(char const *s) const
        {
            return __Hash_String(s);
        }
    };

    template <>
    struct Hash<char>
    {
        size_t operator()(char x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<unsigned char>
    {
        size_t operator()(unsigned char x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<short>
    {
        size_t operator()(short x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<unsigned short>
    {
        size_t operator()(unsigned short x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<int>
    {
        size_t operator()(int x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<unsigned int>
    {
        size_t operator()(unsigned int x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<long>
    {
        size_t operator()(long x) const
        {
            return x;
        }
    };

    template <>
    struct Hash<unsigned long>
    {
        size_t operator()(unsigned long x) const
        {
            return x;
        }
    };

	template <class T>
	struct GetKey {};

	template <>
	struct GetKey<int>
	{
		int operator()(int x) { return x; }
	};

	template <>
	struct GetKey<std::string>
	{
		char const *operator()(std::string const &str) { return str.c_str(); }
	};

	template <class T>
	struct Equal
	{
		bool operator()(T const &a, T const &b) { return a == b; }
	};

	template <class T>
	struct GreaterThan
	{
		bool operator()(T const &a, T const& b) { return a > b; }
	};

	template <class T>
	struct LessThan
	{
		bool operator()(T const &a, T const& b) { return a < b; }
	};
}