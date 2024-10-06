/**
 * UString.cpp
 * A universal string class that can be used to represent both UTF-8 and UTF-16
 * encoded strings and work with them at the same time.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "UString.h"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <exception>

/**
 * Initializes an empty universal Unicode string.
 */
UString::UString() {
	m_mbstr = NULL;
	m_wstr = NULL;
	m_length = 0;
}

/**
 * Initializes a universal Unicode string from an UTF-8 encoded multi-byte C
 * string.
 *
 * @param mbstr UTF-8 encoded multi-byte C string.
 */
UString::UString(const char *mbstr) {
	m_wstr = NULL;
	SetString(_strdup(mbstr));
}

/**
 * Initializes a universal Unicode string from an UTF-16 encoded wide C string.
 *
 * @param wstr UTF-16 encoded wide C string.
 */
UString::UString(const wchar_t *wstr) {
	m_mbstr = NULL;
	SetString(_wcsdup(wstr));
}

/**
 * Cleans up any memory dynamically allocated by us.
 */
UString::~UString() {
	m_length = 0;

	if (m_mbstr) {
		free(m_mbstr);
		m_mbstr = NULL;
	}

	if (m_wstr) {
		free(m_wstr);
		m_wstr = NULL;
	}
}

/**
 * Takes ownership of an UTF-8 encoded multi-byte C string.
 *
 * @param mbstr String to take ownership of. This pointer will now be handled
 *              by this object, do not free or modify it outside of the object.
 */
void UString::TakeOwnership(char *mbstr) {
	SetString(mbstr);
}

/**
 * Takes ownership of an UTF-16 encoded wide string.
 *
 * @param wstr String to take ownership of. This pointer will now be handled by
 *             this object, do not free or modify it outside of the object.
 */
void UString::TakeOwnership(wchar_t *wstr) {
	SetString(wstr);
}

/**
 * Sets the internal multi-byte string and performs all the necessary operations
 * to keep consistency inside the object.
 *
 * @param mbstr New base string.
 */
void UString::SetString(char *mbstr) {
	// If we already have something in our UTF-8 string it should be free'd.
	if (m_mbstr)
		free(m_mbstr);

	// If we already have something in our UTF-16 string it should also go.
	if (m_wstr) {
		free(m_wstr);
		m_wstr = NULL;
	}

	// Set the UTF-8 string and cache its length.
	m_mbstr = mbstr;
	m_length = strlen(mbstr);
}

/**
 * Sets the internal multi-byte string and performs all the necessary operations
 * to keep consistency inside the object.
 *
 * @param wstr New base string.
 */
void UString::SetString(wchar_t *wstr) {
	// If we already have something in our UTF-16 string it should be free'd.
	if (m_wstr)
		free(m_wstr);

	// If we already have something in our UTF-8 string it should also go.
	if (m_mbstr) {
		free(m_mbstr);
		m_mbstr = NULL;
	}

	// Set the UTF-8 string and cache its length.
	m_wstr = wstr;
	m_length = wcslen(wstr);
}

/**
 * Gets the internal multi-byte C string for interoperability with C libraries.
 *
 * @return Pointer to the internal C string. Pointer changes whenever the
 *         contents of the object change.
 */
const char *UString::c_mbstr() {
	// Check if we have a string to return.
	if (m_mbstr == NULL) {
		// Check if we have no string at all.
		if (m_wstr == NULL)
			return NULL;

		// Perform a conversion to make the string available.
		m_mbstr = ToMultiByteString(m_wstr);
	}

	return const_cast<const char *>(m_mbstr);
}

/**
 * Converts an UTF-8 multi-byte C string into an UTF-16 wide string.
 *
 * @warning This method allocates memory dynamically.
 *
 * @param mbstr UTF-8 encoded multi-byte string.
 *
 * @return UTF-16 encoded wide string.
 */
wchar_t *UString::ToWideString(const char *mbstr) {
	wchar_t *wstr;

#ifdef _WIN32
	int nLen;

	/* Get required buffer size and allocate some memory for it. */
	wstr = NULL;
	nLen = MultiByteToWideChar(CP_OEMCP, 0, mbstr, -1, NULL, 0);
	if (nLen == 0)
		goto failure;
	wstr = (wchar_t *)malloc(nLen * sizeof(wchar_t));
	if (wstr == NULL)
		return NULL;

	/* Perform the conversion. */
	nLen = MultiByteToWideChar(CP_OEMCP, 0, mbstr, -1, wstr, nLen);
	if (nLen == 0) {
failure:
		if (wstr)
			free(wstr);
		throw std::exception("Failed to convert UTF-8 string to UTF-16");
	}
#else
	size_t len;

	/* Allocate some memory for our converted string. */
	len = mbstowcs(NULL, mbstr, 0) + 1;
	wstr = (wchar_t *)malloc(len * sizeof(wchar_t));
	if (wstr == NULL)
		return NULL;

	/* Perform the string conversion. */
	len = mbstowcs(wstr, mbstr, len);
	if (len == (size_t)-1) {
		free(wstr);
		throw std::exception("Failed to convert UTF-8 string to UTF-16");
	}
#endif // _WIN32

	return wstr;
}

/**
 * Converts an UTF-16 wide string into an UTF-8 multi-byte C string.
 *
 * @warning This method allocates memory dynamically.
 *
 * @param wstr UTF-16 encoded wide string.
 *
 * @return UTF-8 encoded multi-byte string.
 */
char *UString::ToMultiByteString(const wchar_t *wstr) {
	char *mbstr;
	int nLen;

	/* Get required buffer size and allocate some memory for it. */
	nLen = WideCharToMultiByte(CP_OEMCP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		goto failure;
	mbstr = (char *)malloc(nLen * sizeof(char));
	if (mbstr == NULL)
		return NULL;

	/* Perform the conversion. */
	nLen = WideCharToMultiByte(CP_OEMCP, 0, wstr, -1, mbstr, nLen, NULL, NULL);
	if (nLen == 0) {
failure:
		throw std::exception("Failed to convert UTF-16 string to UTF-8");
	}

	return mbstr;
}

/**
 * Gets the internal wide string for interoperability with C libraries.
 *
 * @return Pointer to the internal wide string. Pointer changes whenever the
 *         contents of the object change.
 */
const wchar_t *UString::c_wstr() {
	// Check if we have a string to return.
	if (m_wstr == NULL) {
		// Check if we have no string at all.
		if (m_mbstr == NULL)
			return NULL;

		// Perform a conversion to make the string available.
		m_wstr = ToWideString(m_mbstr);
	}

	return const_cast<const wchar_t *>(m_wstr);
}

/**
 * Gets the length of the string.
 *
 * @return Length of the string.
 */
size_t UString::Length() {
	return m_length;
}
