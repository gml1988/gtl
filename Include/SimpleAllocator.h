#pragma once

#include <crtdefs.h>
#include <new.h>
#include <iostream>

namespace GTL
{
    template <class T>
    inline T* Allocate(ptrdiff_t size, T*)
    {
        set_new_handler(nullptr);
        T* tmp = (T*)(::operator new((size_t)(size*sizeof(T)));
        if (nullptr == tmp)
        {
            std::cerr << "It's out of memory now" << std::endl;
            exit(1);
        }

        return tmp;
    }
	                
    template <class T>
    void Deallocate(T* buffer)
    {
        ::operator delete(buffer);
    }

    template <class T>
    class SimpleAllocator
    {
    public:
        typedef T               value_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;
        
        template <class U>
        struct rebind
        {
            typedef SimpleAllocator<U>      other;
        }

    public:
        SimpleAllocator::SimpleAllocator() {};
        SimpleAllocator::~SimpleAllocator() {};

    public:
        pointer allocate(size_type n, void* hint = 0)
        {
            return nullptr;
        }

        void deallocate(pointer p, size_type n)
        {
            Deallocate(p);
        }

        void construct(pointer p, const value_type& value)
        {
            
        }

        void destroy(pointer p)
        {
        
        }

        pointer address(reference x)
        {
            return nullptr;
        }

        const_pointer const_address(reference x)
        {
            return nullptr;
        }

        size_type max_size()
        {
            return size_type(UINT_MAX / sizeof(T));
        }
    };
}