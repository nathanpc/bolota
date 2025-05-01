/**
 * tchar.h
 * A shim for the classic TCHAR definitions under Windows.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_SHIM_TCHAR_H
#define _BOLOTA_SHIM_TCHAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>

#ifdef __cplusplus

#include <string>

/**
 * Universal C++ std::string.
 */
#define tstring std::string

extern "C" {

#endif // __cplusplus

/**
 * Unicode text wrapper.
 */
#define _T(str) str

/**
 * Universal character definition.
 */
typedef char TCHAR;

/**
 * Universal string definition.
 */
typedef char* LPTSTR;

/**
 * Universal constant pointer string definition.
 */
typedef const char* LPCTSTR;

/* stdio non-standard naming. */
#define _sntprintf snprintf
#define _stprintf sprintf
#define _strdup strdup
#define _wcsdup wcsdup
#define _tcsdup strdup


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _BOLOTA_SHIM_TCHAR_H
