#pragma once

namespace
{
    // Solution: Traverse from two ends to the middle
    bool IsPalindromeNumber(int nNum)
    {
        // The negative number has been excluded first, there is no overflow case for nDiv
        if (nNum < 0)
        {
            return false;
        }

        int nDiv = 1;
        int nTmp = nNum;
        while (nTmp < 10)
        {
            nTmp = nTmp/10;
            nDiv *= 10;
        }

        nTmp = nNum;
        while (nTmp)
        {
            // Judge whether the highest digit equals the lowest digit
            if (nTmp % nDiv != nTmp % 10)
            {
                return false;
            }
            // Remove the highest digit and lowest digit
            nTmp = (nTmp % nDiv) / 10;
            // Removed two digit, the magnitude has decreased 2
            nDiv = nDiv % 100;
        }

        return true;
    }
}