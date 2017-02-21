// \brief
//		basic data types definition
//

#ifndef __JETX_H__
#define __JETX_H__

#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t
#include <stddef.h> // ptrdiff_t
#include <string.h> // memcpy
#include <limits.h>
#include <cassert>
#include <algorithm>

#include "XMemory.h"


#define STRUCT_VAR_OFFSET(cls, var)		((char *)&(((cls*)0)->var) - ((char *)0))
#define STATIC_ARRAY_COUNT(a)			(sizeof(a)/sizeof(a[0]))

#endif // __JETX_H__
