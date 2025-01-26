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
	#if defined(UNDER_CE) && !defined(WIN32_PLATFORM_PSPC) && !defined(WIN32_PLATFORM_WFSP)
		#include <wce_string.h>
	#endif // UNDER_CE && !WIN32_PLATFORM_PSPC && !WIN32_PLATFORM_WFSP
#endif // _WIN32

#include "../../shims/cvtutf/Unicode.h"
#include "Errors/Error.h"

/**
 * Initializes an empty universal Unicode string.
 */
UString::UString() {
	Initialize();
}

/**
 * Initializes a universal Unicode string from an UTF-8 encoded multi-byte C
 * string.
 *
 * @param mbstr UTF-8 encoded multi-byte C string.
 */
UString::UString(const char *mbstr) {
	Initialize();
	SetString(_strdup(mbstr));
}

/**
 * Initializes a universal Unicode string from an UTF-16 encoded wide C string.
 *
 * @param wstr UTF-16 encoded wide C string.
 */
UString::UString(const wchar_t *wstr) {
	Initialize();
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
 * Initializes the object with some sane defaults.
 */
void UString::Initialize() {
	m_mbstr = NULL;
	m_wstr = NULL;
	m_length = 0;
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
	m_length = (mbstr) ? strlen(mbstr) : 0;
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
	m_length = (wstr) ? wcslen(wstr) : 0;
}

/**
 * Gets the internal multi-byte C string for interoperability with C libraries.
 *
 * @return Pointer to the internal C string. Pointer changes whenever the
 *         contents of the object change.
 */
const char *UString::GetMultiByteString() {
	// Check if we have a string to return.
	if (m_mbstr == NULL) {
		// Check if we have no string at all.
		if (m_wstr == NULL)
			return NULL;

		// Perform a conversion to make the string available.
		m_mbstr = ToMultiByteString(m_wstr);

		// Ensure we base our length from this string.
		m_length = strlen(m_mbstr);
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
	wchar_t *wstr = NULL;

	if (!Unicode::MultiByteToWideChar(mbstr, &wstr)) {
		ThrowError(EMSG("Failed to convert UTF-8 string to UTF-16"));
		return BOLOTA_ERR_NULL;
	}

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
	char *mbstr = NULL;

	if (!Unicode::WideCharToMultiByte(wstr, &mbstr)) {
		ThrowError(EMSG("Failed to convert UTF-16 string to UTF-8"));
		return BOLOTA_ERR_NULL;
	}

	return mbstr;
}

/**
 * Gets the internal wide string for interoperability with C libraries.
 *
 * @return Pointer to the internal wide string. Pointer changes whenever the
 *         contents of the object change.
 */
const wchar_t *UString::GetWideString() {
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
 * Gets a string in the native format of the platform (set at compile time).
 *
 * @return String in the native (at compile time) format of the platform.
 */
const TCHAR *UString::GetNativeString() {
#ifdef UNICODE
	return GetWideString();
#else
	return GetMultiByteString();
#endif // UNICODE
}

/**
 * Gets the length of the string.
 *
 * @return Length of the string.
 */
size_t UString::Length() {
	// Ensure we have the UTF-8 string to get its length.
	if (m_mbstr == NULL)
		GetMultiByteString();

	return m_length;
}

/**
 * Checks if the string is currently empty.
 *
 * @return TRUE if the string is empty, FALSE otherwise.
 */
bool UString::Empty() const {
	return m_length == 0;
}

/**
 * Sets the string contents using a multi-byte character string.
 *
 * @param mbstr Multi-byte character string.
 */
UString& UString::operator=(const char *mbstr) {
	SetString(_strdup(mbstr));
	return *this;
}

/**
 * Sets the string contents using a wide character string.
 *
 * @param wstr Wide character string.
 */
UString& UString::operator=(const wchar_t *wstr) {
	SetString(_wcsdup(wstr));
	return *this;
}
