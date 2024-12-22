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

#include "../../stdafx.h"
#include "Error.h"

/**
 * A C++ wrapper for GetLastError() in Win32 API.
 *
 * @see GetLastError
 */
class SystemError : public Bolota::Error {
protected:
	DWORD m_dwLastError;
	TCHAR *m_szLastErrorMessage;

	// Constructor helper.
	void Initialize(const TCHAR *szMessage);

public:
	// Constructors and destructors.
	SystemError(const TCHAR *szMessage);
	SystemError();
	virtual ~SystemError();

	// Message operations.
	void RefreshMessage(const TCHAR *szMessage);
	const TCHAR* LastErrorMessage() const;
};

#endif // _BOLOTA_ERRORS_SYSTEMERROR_H
