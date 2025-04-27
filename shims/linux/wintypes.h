/**
 * wintypes.h
 * A shim for a bunch of very common types used in Windows.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_SHIM_WINTYPES_H
#define _BOLOTA_SHIM_WINTYPES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * DWORD on Windows becomes size_t.
 */
typedef size_t DWORD;

/**
 * 8 bits of unsigned integer glory.
 */
typedef uint8_t UINT8;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _BOLOTA_SHIM_WINTYPES_H
