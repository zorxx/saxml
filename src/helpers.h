/*! \copyright 2025 Zorxx Software. All rights reserved.
 *  \license This file is released under the MIT License. See the LICENSE file for details.
 */
#ifndef SAXML_HELPERS_H
#define SAXML_HELPERS_H

#define UNUSED(x) (void)x;

/* Debugging without the use of variadic macros to support C90 */
#if defined(SAXML_ENABLE_DEBUG)
   #include <stdio.h>
   #define DBG(s) fprintf(stderr, s)
   #define DBG1(s, a) fprintf(stderr, s, a)
#else
   #define DBG(s)
   #define DBG1(s, a)
#endif

#endif /* SAXML_HELPERS_H */
