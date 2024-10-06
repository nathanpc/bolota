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
#include <wchar.h>

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
	const char *c_mbstr();
	const wchar_t *c_wstr();

	// Getters
	size_t Length();

protected:
	// Setters for internal strings.
	void SetString(char *mbstr);
	void SetString(wchar_t *wstr);

	// Encoding converters.
	static wchar_t *ToWideString(const char *mbstr);
	static char *ToMultiByteString(const wchar_t *wstr);
};

#endif // _INNOVE_USTRING_H
