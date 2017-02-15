#pragma once

#include "Gtl_Common.h"

namespace GTL
{
	/*
	Max-Heap: The parent node is always greater or equal than the children
	Push-Heap: Direction: Sift up from the leaf node -> the root node until get the proper location
	Pop-Heap: Swap the root and the last node, and Sift the root node down to the proper location
	*/

	// For the max heap, the comparer is >, not >=
	// For the max heap, the comparer is <, not <=
	// Insert the obj to the location of the last iterator point to
	template <class RandomAccessIterator, class ValueComparer, class T>
	inline void PushHeap(RandomAccessIterator first, RandomAccessIterator last, T const& obj, ValueComparer &comparer)
	{
		*(last++) = obj;
		PushHeap(first, last, comparer);
	}

    template <class RandomAccessIterator, class ValueComparer, class T>
	inline void PushHeap(RandomAccessIterator first, RandomAccessIterator last, ValueComparer &comparer)
	{
		size_t nDistance = last - first - 1;
		__SiftUp(first, nDistance, comparer);
	}

	template <class RandomAccessIterator, class Distance, class ValueComparer>
	inline void __SiftUp(RandomAccessIterator first, Distance distIndex, ValueComparer &comparer)
	{
		while (distIndex > 0)
		{
			Distance distParentIndex = (distIndex - 1) >> 1;
			if (comparer(*(first + distIndex), *(first + distParentIndex)))
			{
				Swap(*(first + distIndex), *(first + distParentIndex));
				distIndex = distParentIndex;
			}
			else
			{
				break;
			}
		}
	}

	// [first, last)
	template <class RandomAccessIterator, class ValueComparer>
	inline void PopHeap(RandomAccessIterator first, RandomAccessIterator last, ValueComparer &comparer)
	{
		--last;
		if (first < last)
		{
			Swap(*first, *last);
		}
		__SiftDown(first, last - first, comparer);
	}

	template <class RandomAccessIterator, class Distance, class ValueComparer>
	inline void __SiftDown(RandomAccessIterator first, Distance size, ValueComparer &comparer)
	{
		Distance dist = 0;
		while (dist < size)
		{
			Distance distTarget = dist;
			Distance distLeftChild = (dist << 1) + 1;
			Distance distRightChild = (dist << 1) + 2;
			// Pick the max element from the current, leftchild and rightchild
			if ((distLeftChild < dist) && comparer(*(first + distLeftChild), *(first + distTarget)))
			{
				distTarget = distLeftChild;
			}
			if ((distRightChild < dist) && comparer(*(first + distRightChild), *(first + distTarget)))
			{
				distTarget = distRightChild;
			}
			// If the max element is from the children, then swap and continue SiftDown, else break
			if (distTarget != dist)
			{
				Swap(*(first + distTarget), *(first + dist));
                dist = distTarget;
			}
			else
			{
				break;
			}
		}
	}

	template <class RandomAccessIterator, class ValueComparer>
	inline void MakeHeap(RandomAccessIterator first, RandomAccessIterator last, ValueComparer &comparer)
	{
		size_t nSize = last - first;
		size_t nLastElemIndex = 0;
		while (nLastElemIndex < nSize)
		{
			__SiftUp(first, nLastElemIndex++, comparer);
		}
	}

	template <class RandomAccessIterator, class ValueComparer>
	inline void HeapSort(RandomAccessIterator first, RandomAccessIterator last, ValueComparer &comparer)
	{
		MakeHeap(first, last, comparer);
		while (first < last)
		{
			PopHeap(first, last, comparer);
			--last;
		}
	}
}