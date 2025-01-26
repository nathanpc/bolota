/**
 * Unicode.h
 * A wrapper for Unicode's CVTUTF string conversion library.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _SHIMS_CVTUTF_WRAPPER_H
#define _SHIMS_CVTUTF_WRAPPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wchar.h>

namespace Unicode {
	bool WideCharIsUTF16();
	bool MultiByteToWideChar(const char* mbstr, wchar_t** wstr);
}

#endif // _SHIMS_CVTUTF_WRAPPER_H
