#pragma once

#include "Iterator.h"
#include "Iterator_Traits.h"

namespace GTL
{
	template <class InputIterator, class Distance>
	inline void __Advance(InputIterator& iter, Distance dist, input_iterator_tag)
	{
		while (dist--)
		{
			++iter;
		}
	}

	template <class InputIterator, class Distance>
	inline void __Advance(InputIterator& iter, Distance dist, bidirectional_iterator_tag)
	{
		if (dist >= 0)
		{
			while (dist--)
			{
				++iter;
			}
		}
		else
		{
			while (dist++)
			{
				--iter;
			}
		}
	}

    template <class InputIterator, class Distance>
    inline void __Advance(InputIterator& iter, Distance dist, random_access_iterator_tag)
    {
        iter += dist;
    }

    template <class InputIterator, class Distance>
	inline void Advance(InputIterator& iter, Distance dist)
	{
		__Advance(iter, dist, Iterator_Category(iter));
	}
}