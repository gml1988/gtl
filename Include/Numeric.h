#pragma once

namespace GTL
{
    template <class T>
    inline T Max(T const &valueA, T const &valueB)
    {
        return valueA > valueB ? valueA : valueB;
    }

    template <class T>
    inline T Min(T const &valueA, T const &valueB)
    {
        return valueA < valueB ? valueA : valueB;
    }

    template <class TV, class TM>
    inline TV RoundDown(TV const &value, TM const &multiple)
    {
        return ((value / multiple) * multiple);
    }

    template <class TV, class TM>
    inline TV RoundUp(TV const &value, TM const &multiple)
    {
        return (RoundDown(value, multiple) + 
            (value % multiple) ? 1 : 0);
    }
}