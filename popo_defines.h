#ifndef POPO_DEFINES_H
#define POPO_DEFINES_H

// Reference: https://sourceforge.net/p/predef/wiki/OperatingSystems/
// Reference: https://sourceforge.net/p/predef/wiki/Compilers/

/* ============================================================================================== */
/* Compiler detection                                                                             */
/* ============================================================================================== */
                                                                                
#if defined(__GNUC__)
#   define POPO_GCC
#   define POPO_GNUC
#elif defined(_MSC_VER)                                                         
#   define POPO_MSVC                                                            
#elif defined(__clang__)                                                          
#   define POPO_CLANG                                                           
#   define POPO_GNUC                                                            
#else                                                                           
#   define POPO_UNKNOWN_COMPILER                                                
#endif                   

#if !defined(__GLIBC__)
# define POPO_NO_LIBC
#endif

#define POPO_FAILURE(x) (x == POPO_STATUS_FAILURE)
#define POPO_SUCCESS(x) (x == POPO_STATUS_SUCCESS)

#define POPO_ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))
#define POPO_NULL ((void*)0)
#define POPO_UNUSED(x) (void)(x)

#endif /* POP_DEFINES_H */
