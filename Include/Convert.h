#include <string>

class Convert
{
public:
    static std::string ToString(std::string strFormat, int nNum)
    {
        if ("%d" == strFormat)
        {
            return Convert::ToBinaryString(nNum);
        }
    }

    static int ToInt32(std::string str)
    {
        return atoi(str.c_str());
    }

    static unsigned int ToUInt32(std::string str)
    {
        
    }

protected:
    static std::string ToBinaryString(int nNum)
    {
        std::string strRet;
        // Insert the character to the string from the lower bit to the higher bit
        while (nNum)
        {
            strRet += (nNum & 1) ? '1' : '0';
            nNum = (nNum >> 1) & 0x7FFFFFFF;
        }

        // Reverse the string expression, the str is from the higher bit to the lower bit
        int nSize = strRet.size();
        for (int iIndex = 0; iIndex < nSize >> 1; ++iIndex)
        {
            std::swap(strRet[iIndex], strRet[nSize - iIndex - 1]);
        }

        return strRet;
    }

    static std::string ToOctalString(int nNum)
    {
    
    }

    static std::string ToHexadecimalString(int nNum)
    {
        
    }
};