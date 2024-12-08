/**
 * newcpp.h
 * Modernizes old C++ compilers, or at least makes them ignore the newflanged
 * things.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _SHIMS_NEWCPP_H
#define _SHIMS_NEWCPP_H
#ifdef __cplusplus

/* Ensure we have proper standard numerical types. */
#include <stdint.h>

#if _MSC_VER <= 1200

/* Override keyword for virtual methods will be ignored. */
#define override

#endif /* _MSC_VER <= 1200 */

#endif /* __cplusplus */
#endif /* _SHIMS_NEWCPP_H */
