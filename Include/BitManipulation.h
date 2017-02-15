#include <assert.h>

// Swap every 2 bits
// Swap every 4 bits
// Swap every 8 bits
// Swap every 16 bits
// Swap every 32 bits
int ReverseBits(int nNum)
{
    assert(4 == sizeof(nNum));  

    nNum = ((0x55555555 & nNum) << 1) | ((0xAAAAAAAA & nNum) >> 1);
    nNum = ((0x33333333 & nNum) << 2) | ((0xCCCCCCCC & nNum) >> 2);
    nNum = ((0x0F0F0F0F & nNum) << 4) | ((0xF0F0F0F0 & nNum) >> 4);
    nNum = ((0x00FF00FF & nNum) << 8) | ((0xFF00FF00 & nNum) >> 8);
    nNum = ((0x0000FFFF & nNum) << 16) | ((0xFFFF0000 & nNum) >> 16);

    return nNum;
}

int SwapBits(int nIndexA, int nIndexB, int nNum)
{
    if (((nNum & (1 << nIndexA)) >> nIndexA) ^ ((nNum & (1 << nIndexB)) >> nIndexB))
    {
        nNum = nNum ^ ((1 << nIndexA) | (1 << nIndexB));
    }

    return nNum;
}

