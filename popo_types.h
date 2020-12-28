#ifndef POPO_TYPES_H
#define POPO_TYPES_H

#include "popo_defines.h"

// TODO: add support for other platforms i.e Windows

// Reference: https://sourceforge.net/p/predef/wiki/OperatingSystems/
// Reference: https://sourceforge.net/p/predef/wiki/Compilers/

#if defined(POPO_NO_LIBC)
#   if defined(POPO_GNUC)
        typedef __UINT8_TYPE__   U8;
        typedef __UINT16_TYPE__  U16;
        typedef __UINT32_TYPE__  U32;
        typedef __UINT64_TYPE__  U64;
        typedef __INT8_TYPE__    I8;
        typedef __INT8_TYPE__    CHAR;
        typedef __INT16_TYPE__   I16;
        typedef __INT32_TYPE__   I32;
        typedef __INT64_TYPE__   I64;
        typedef __SIZE_TYPE__    USize;
        typedef __PTRDIFF_TYPE__ ISize;
        typedef __UINTPTR_TYPE__ UPointer;
        typedef __INTPTR_TYPE__  IPointer;
#   else
#       error "Unsupported compiler for no-libc mode."
#   endif
#else
    // If is libc present, we use stdint types.
#   include <stdint.h>
#   include <stddef.h>
    typedef uint8_t   U8;
    typedef uint16_t  U16;
    typedef uint32_t  U32;
    typedef uint64_t  U64;
    typedef int8_t    I8;
    typedef int8_t    CHAR;
    typedef int16_t   I16;
    typedef int32_t   I32;
    typedef int64_t   I64;
    typedef size_t    USize;
    typedef ptrdiff_t ISize;
    typedef uintptr_t UPointer;
    typedef intptr_t  IPointer;
#endif

typedef enum _PopoBool {
    POPO_FALSE,
    POPO_TRUE
} PopoBool;

typedef enum _PopoStatus {
    POPO_STATUS_FAILURE,
    POPO_STATUS_SUCCESS
} PopoStatus;

typedef struct _PopoArgs {
    I32 pid;
    char* image_path;
} PopoArgs;


#endif /* POPO_TYPES_H */
