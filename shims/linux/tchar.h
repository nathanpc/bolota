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


#define _sntprintf snprintf


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _BOLOTA_SHIM_TCHAR_H
