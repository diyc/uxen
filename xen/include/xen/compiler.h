#ifndef __LINUX_COMPILER_H
#define __LINUX_COMPILER_H

#if defined(WINNT) && !defined(__GNUC__)

#define __signed__ signed

#else  /* WINNT && !__GNUC__ */

#if !defined(__GNUC__) || (__GNUC__ < 3)
#error Sorry, your compiler is too old/not recognized.
#endif

#define barrier()     __asm__ __volatile__("": : :"memory")

#define likely(x)     __builtin_expect((x),1)
#define unlikely(x)   __builtin_expect((x),0)

#define inline        __inline__
#define always_inline __inline__ __attribute__ ((always_inline))
#define noinline      __attribute__((noinline))

#ifdef INIT_SECTIONS_ONLY
/*
 * For sources indicated to have only init code, make sure even
 * inline functions not expanded inline get placed in .init.text.
 */
#include <xen/init.h>
#define __inline__ __inline__ __init
#endif

#define __attribute_pure__  __attribute__((pure))
#define __attribute_const__ __attribute__((__const__))

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#define __attribute_used__ __attribute__((__used__))
#else
#define __attribute_used__ __attribute__((__unused__))
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define __must_check __attribute__((warn_unused_result))
#else
#define __must_check
#endif

#if __GNUC__ > 3
#if !defined(offsetof)
#define offsetof(a,b) __builtin_offsetof(a,b)
#endif
#else
#define offsetof(a,b) ((unsigned long)&(((a *)0)->b))
#endif

/* &a[0] degrades to a pointer: a different type from an array */
#define __must_be_array(a) \
  BUILD_BUG_ON_ZERO(__builtin_types_compatible_p(typeof(a), typeof(&a[0])))

#ifdef GCC_HAS_VISIBILITY_ATTRIBUTE
/* Results in more efficient PIC code (no indirections through GOT or PLT). */
#pragma GCC visibility push(hidden)
#endif

/* This macro obfuscates arithmetic on a variable address so that gcc
   shouldn't recognize the original var, and make assumptions about it */
/*
 * Versions of the ppc64 compiler before 4.1 had a bug where use of
 * RELOC_HIDE could trash r30. The bug can be worked around by changing
 * the inline assembly constraint from =g to =r, in this particular
 * case either is valid.
 */
#define RELOC_HIDE(ptr, off)                    \
  ({ unsigned long __ptr;                       \
    __asm__ ("" : "=r"(__ptr) : "0"(ptr));      \
    (typeof(ptr)) (__ptr + (off)); })

#ifndef __OBJ_PE__
#ifndef __ASSEMBLY__
#define _ASM_SUBSECTION ".subsection"
#define _ASM_PREVIOUS ".previous"
#else
#define _ASM_SUBSECTION .subsection
#define _ASM_PREVIOUS .previous
#endif
#else  /* __OBJ_PE__ */
#ifndef __ASSEMBLY__
#define _ASM_SUBSECTION ".text"
#define _ASM_PREVIOUS ".text"
#else
#define _ASM_SUBSECTION .text
#define _ASM_PREVIOUS .text
#endif
#endif /* __OBJ_PE__ */

#ifndef WINNT
#define __PRINTF_FORMAT printf
#define __SCANF_FORMAT scanf
#else
#define __PRINTF_FORMAT gnu_printf
#define __SCANF_FORMAT gnu_scanf
#endif

#ifdef WINNT
#define sizeof(x) ((uint64_t)sizeof(x))
#endif

#endif	/* WINNT && !__GNUC__ */

#endif /* __LINUX_COMPILER_H */
