/**
 * SystemError.h
 * An error that is thrown in order to wrap a system error.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_ERRORS_SYSTEMERROR_H
#define _BOLOTA_ERRORS_SYSTEMERROR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
	#include "../../stdafx.h"
#else
	#include <winfile.h>
#endif // _WIN32

#include "Error.h"

/**
 * A C++ wrapper for GetLastError() in Win32 API.
 *
 * @see GetLastError
 */
class SystemError : public Bolota::Error {
protected:
	TCHAR *m_szLastErrorMessage;
#ifdef _WIN32
	DWORD m_dwLastError;
#else
	int m_iError;
#endif // _WIN32

	// Constructor helper.
#ifdef _WIN32
	void Initialize(const TCHAR *szMessage, DWORD dwError);
#else
	void Initialize(const TCHAR *szMessage, int iError);
#endif // _WIN32

public:
	// Constructors and destructors.
	SystemError(const TCHAR* szMessage);
#ifdef _WIN32
	SystemError(const TCHAR* szMessage, DWORD dwError);
#endif // _WIN32
	SystemError();
	virtual ~SystemError();

	// Message operations.
	void RefreshMessage(const TCHAR *szMessage);
	const TCHAR* LastErrorMessage() const;
};

#endif // _BOLOTA_ERRORS_SYSTEMERROR_H
