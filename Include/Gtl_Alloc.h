#pragma once

#include <stdlib.h>

namespace GTL
{
    typedef void (*MallocHandler)();

    // The 1st level memory allocator
    template <int inst>
    class __Malloc_Alloc_Template
    {
    public:
        static void *Allocate(size_t n)
        {
            void *tmp = malloc(n);
            if (nullptr == tmp)
            {
                tmp = _OOM_Malloc(n);
            }

            return tmp;
        }

        static void Deallocate(void* p, size_t)
        {
            free(p);
        }

        static void *Reallocate(void* p, size_t, size_t nNewSize)
        {
            void *tmp = realloc(p, nNewSize);
            if (nullptr == tmp)
            {
                tmp = _OOM_Realloc(p, nNewSize);
            }

            return tmp;
        }

        static MallocHandler SetMallocHandler(MallocHandler pfnHandler)
        {
            MallocHandler pfnOldHandler = _pfnMalloc_Alloc_oom_Handler;

            _pfnMalloc_Alloc_oom_Handler = pfnHandler;

            return pfnOldHandler;
        }

    private:
        static void *_OOM_Malloc(size_t nSize)
        {
            MallocHandler pfnHandler;
            void *pvResult = nullptr;

            // Continue trying to allocate the memory until success
            while (true)
            {
                pfnHandler = _pfnMalloc_Alloc_oom_Handler;
                if (nullptr == pfnHandler)
                {
                    //_THROW_BAD_ALLOC;
                }
                pfnHandler();
                pvResult = malloc(nSize);
                if (nullptr != pvResult)
                {
                    return pvResult;
                }
            }

            return pvResult;
        }

        static void *_OOM_Realloc(void *p, size_t nSize)
        {
            MallocHandler pfnHandler;
            void *pvResult = nullptr;

            // Continue trying to reallocate the memory until success
            while (true)
            {
                pfnHandler = _pfnMalloc_Alloc_oom_Handler;
                if (nullptr == pfnHandler)
                {
                    //_THROW_BAD_ALLOC;
                }
                pfnHandler();
                pvResult = realloc(p, nSize);
                if (nullptr != pvResult)
                {
                    return pvResult;
                }
            }

            return pvResult;
        }

    private:
        static void (*_pfnMalloc_Alloc_oom_Handler)();
    };

    template <int inst>
    MallocHandler __Malloc_Alloc_Template<inst>::_pfnMalloc_Alloc_oom_Handler = nullptr;

    typedef __Malloc_Alloc_Template<0> Malloc_Alloc;

    enum {__ALIGN = 8};
    enum {__MAX_BYTES = 128};
    enum {__NFREELISTS = __MAX_BYTES/__ALIGN};
    union obj
    {
        union obj *pFreeListLink;
        char clientData[1];
    };
    // The 2nd level memory allocator
    template <bool threads, int inst>
    class __Default_Alloc_Template
    {
    public:
        static void *Allocate(size_t nSize)
        {
            void *pvResult = nullptr;

            // First try to judege the size of the memory to be allcated
            // If the memory needed is larger than the threshold, allocate the memory from the 1st level allocator
            if (nSize > __MAX_BYTES)
            {
                pvResult = Malloc_Alloc::Allocate(nSize);
                return pvResult;
            }
            // Try to get the memory from the free list link
            size_t nIndex = _GetFreeListIndex(nSize);
            pvResult = _rgFreeList[nIndex];
            if (nullptr == pvResult)
            {
                size_t nMemorySizeToAlloc = _RoundUp(nSize);
                pvResult = _Refill(nMemorySizeToAlloc);
            }
            _rgFreeList[nIndex] = _rgFreeList[nIndex]->pFreeListLink;

            return pvResult;
        }

        static void *Reallocate(void *pvMemory, size_t nOldSize, size_t nNewSize)
        {
            void *pvResult = nullptr;

            return pvResult;
        }

        static void Deallocate(void *pvMemory, size_t nSize)
        {
            // If the memory to recycle is larger than __MAX_BYTES, use the 1st level allocator to deallocate it
            if (nSize > __MAX_BYTES)
            {
                Malloc_Alloc::Deallocate(pvMemory, nSize);
            }
            else
            {
                size_t nIndexInFreeList = _GetFreeListIndex(nSize);
                (static_cast<obj *>(pvMemory))->pFreeListLink = _rgFreeList[nIndexInFreeList];
                _rgFreeList[nIndexInFreeList] = static_cast<obj *>(pvMemory);
            }
        }
        
    private:
        static size_t _RoundUp(size_t nSize)
        {
            return (nSize + __ALIGN - 1) & ~(__ALIGN - 1);
        }

        static size_t _GetFreeListIndex(size_t nSize)
        {
            // nSize/__ALIGH -> 0 ~ __ALIGN-1
            // 1 ~ __ALIGN need 
            return ((nSize + __ALIGN - 1)/__ALIGN - 1);
        }

        static void *_Refill(size_t nSize)
        {
            size_t nObjNum = 20;
            void *pvResult = nullptr;
            char *pcNewAllocMemory = _ChunkAlloc(nSize, nObjNum);

            // If allocate 0 objects, return null direclty
            // If allocate just 1 object, return the object directly
            if (0 == nObjNum || 1 == nObjNum)
            {
                return pcNewAllocMemory;
            }
            // The first part of the allocated objects is return value
            pvResult = pcNewAllocMemory;
            obj *pObjAllocForFreeStart = reinterpret_cast<obj *>(pcNewAllocMemory + nSize);
            size_t nIndexInFreeList = _GetFreeListIndex(nSize);
            obj *pObjFreeList = _rgFreeList[nIndexInFreeList];
            _rgFreeList[nIndexInFreeList] = pObjAllocForFreeStart;
            // Compose the allocated memory to a list of objs
            obj *pObjTraverse = pObjAllocForFreeStart;
            for (size_t nIndex = 1; nIndex < nObjNum - 1; ++nIndex)
            {
                pObjTraverse->pFreeListLink = reinterpret_cast<obj *>(reinterpret_cast<char *>(pObjTraverse) + nSize);
                pObjTraverse = pObjTraverse->pFreeListLink;
            }
            // Combine the previous part of the free memory to the end of the new allocated memory
            pObjTraverse->pFreeListLink = pObjFreeList;
        
            return pvResult;
        }

        static char *_ChunkAlloc(size_t nUnitSize, size_t &nObjNum)
        {
            char *pcResult = nullptr;
            size_t nTotalBytes = nUnitSize * nObjNum;
            size_t nLeftBytes = _pEndFree - _pStartFree;
        
            if (nLeftBytes >= nTotalBytes)      // First check the memory pool
            {
                pcResult = _pStartFree;
                _pStartFree += nTotalBytes;
                return pcResult;
            }
            else if(nLeftBytes > nUnitSize)     // The memory is enough for accomodate more than one objs
            {
                nObjNum = nLeftBytes / nUnitSize;
                nTotalBytes = nUnitSize * nObjNum;
                pcResult = _pStartFree;
                _pStartFree += nTotalBytes;
                return pcResult;
            }
            else
            {
                size_t nBytesToGet = 2 * nTotalBytes + _RoundUp((_nHeapSize >> 4));
                // First use the remaining memory to the free list
                if (nLeftBytes > 0)
                {
                    size_t nIndexOfLeftBytesInFreeList = _GetFreeListIndex(nLeftBytes);
                    (reinterpret_cast<obj *>(_pStartFree))->pFreeListLink = _rgFreeList[nIndexOfLeftBytesInFreeList];
                    _rgFreeList[nIndexOfLeftBytesInFreeList] = reinterpret_cast<obj *>(_pStartFree);
                }
                // Allocate the memory from the C runtime heap
                _pStartFree = static_cast<char *>(malloc(nBytesToGet));
                _nHeapSize += nBytesToGet;
                _pEndFree = _pStartFree + nBytesToGet;
                return _ChunkAlloc(nUnitSize, nObjNum);
            }
        }

    protected:
        static obj * volatile _rgFreeList[__NFREELISTS];
        static char *_pStartFree;
        static char *_pEndFree;
        static size_t _nHeapSize;
    };

    template <bool threads, int inst>
    char *__Default_Alloc_Template<threads, inst>::_pStartFree = 0;

    template <bool threads, int inst>
    char *__Default_Alloc_Template<threads, inst>::_pEndFree = 0;

    template <bool threads, int inst>
    size_t __Default_Alloc_Template<threads, inst>::_nHeapSize = 0;

    template <bool threads, int inst>
    obj * volatile __Default_Alloc_Template<threads, inst>::_rgFreeList[__NFREELISTS] = 
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
}