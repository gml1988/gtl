#pragma once

#include "Gtl_Alloc.h"

namespace GTL
{
#ifdef __USE_MALLOC
    typedef __Malloc_Alloc_Template<0> Allocator;
#else
    typedef __Default_Alloc_Template<false, 0> Allocator;
#endif

	template <class T, class Alloc>
	class Simple_Allocator
	{
	public:
		static T *Allocate(size_t nSize)
		{
			return (0 == nSize) ? nullptr : (T *)Alloc::Allocate(nSize * sizeof(T));
		}

		static T *Allocate(void)
		{
			return (T *)Alloc::Allocate(sizeof(T));
		}

		static void Deallocate(T *p)
		{
			Alloc::Deallocate(p, sizeof(T));
		}

        static void Deallocate(T *p, size_t nSize)
        {
            Alloc::Deallocate(p, nSize * sizeof(T));
        }
	};
}