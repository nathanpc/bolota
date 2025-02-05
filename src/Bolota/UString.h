/**
 * UString.h
 * A universal string class that can be used to represent both UTF-8 and UTF-16
 * encoded strings and work with them at the same time.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _INNOVE_USTRING_H
#define _INNOVE_USTRING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <string.h>
#include <tchar.h>

/**
 * Definition of the system's preferred line ending sequence.
 */
#ifdef _WIN32
	#define LINEND  "\r\n"
	#define LINENDW L"\r\n"
	#define LINENDT _T("\r\n")
#else
	#define LINEND  "\n"
	#define LINENDW L"\n"
	#define LINENDT LINEND
#endif // _WIN32

/**
 * A universal string class that can be used to represent both UTF-8 and UTF-16
 * encoded strings and work with them at the same time.
 */
class UString {
private:
	char *m_mbstr;
	wchar_t *m_wstr;
	size_t m_length;

public:
	// Constructors and destructors.
	UString();
	UString(const char *mbstr);
	UString(const wchar_t *wstr);
	virtual ~UString();

	// Ownership handling.
	void TakeOwnership(char *mbstr);
	void TakeOwnership(wchar_t *wstr);

	// Access to the internal strings.
	const char *GetMultiByteString();
	const wchar_t *GetWideString();
	const TCHAR *GetNativeString();

	// Free up unused internal strings.
	void FreeMultiByteString();
	void FreeWideString();

	// Encoding converters.
	static wchar_t *ToWideString(const char* mbstr);
	static char *ToMultiByteString(const wchar_t* wstr);

	// Getters
	size_t Length();
	bool Empty() const;

	// Operators
	UString& operator=(const char *mbstr);
	UString& operator=(const wchar_t *wstr);

protected:
	// Constructor helper.
	void Initialize();

	// Setters for internal strings.
	void SetString(char *mbstr);
	void SetString(wchar_t *wstr);
};

#endif // _INNOVE_USTRING_H
