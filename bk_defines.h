#ifndef BK_DEFINES_H
#define BK_DEFINES_H

// Reference: https://sourceforge.net/p/predef/wiki/OperatingSystems/
// Reference: https://sourceforge.net/p/predef/wiki/Compilers/

/* ============================================================================================== */
/* Compiler detection                                                                             */
/* ============================================================================================== */
                                                                                
#if defined(__GNUC__)
#   define BK_GCC
#   define BK_GNUC
#elif defined(_MSC_VER)                                                         
#   define BK_MSVC                                                            
#elif defined(__clang__)                                                          
#   define BK_CLANG                                                           
#   define BK_GNUC                                                            
#else                                                                           
#   define BK_UNKNOWN_COMPILER                                                
#endif                   

#if !defined(__GLIBC__)
# define BK_NO_LIBC
#endif

#define BK_ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))
#define BK_NULL ((void*)0)
#define BK_UNUSED(x) (void)(x)

#endif /* !BK_DEFINES_H */
