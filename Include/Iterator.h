#pragma once

#include "Gtl_Common.h"
#include <crtdefs.h>

namespace GTL
{
    template <class Category, 
              class T, 
              class Distance = ptrdiff_t, 
              class Pointer = T*, 
              class Reference = T&>
    struct iterator
    {
        typedef Category        iterator_category;
        typedef T               value_type;
        typedef Distance        difference_type;
        typedef Pointer         pointer;
        typedef Reference       reference;
    };
}