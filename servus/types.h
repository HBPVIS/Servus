/* Copyright (c) 2015, Human Brain Project
 *                     Stefan.Eilemann@epfl.ch
 *                     Juan Hernando <jhernando@fi.upm.es>
 */

#ifndef SERVUS_TYPES_H
#define SERVUS_TYPES_H

#include "uint128_t.h"
#include "uri.h"

#include <sys/types.h>
#ifndef _MSC_VER
#  include <stdint.h>
#endif
#ifdef _WIN32
#  include <basetsd.h>
#  ifdef _MSC_VER
typedef UINT64     uint64_t;
typedef INT64      int64_t;
typedef UINT32     uint32_t;
typedef INT32      int32_t;
typedef UINT16     uint16_t;
typedef INT16      int16_t;
typedef UINT8      uint8_t;
typedef INT8       int8_t;
#      ifndef HAVE_SSIZE_T
typedef SSIZE_T    ssize_t;
#      define HAVE_SSIZE_T
#    endif
#  endif // Win32, Visual C++
#endif // Win32

#include <vector>
#include <string>

namespace servus
{
typedef std::vector< std::string > Strings;
}

#endif
