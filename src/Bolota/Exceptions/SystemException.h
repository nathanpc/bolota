/**
 * SystemException.h
 * An exception that is thrown in order to wrap a GetLastError() call.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_EXCEPTIONS_SYSTEMEXCEPTION_H
#define _BOLOTA_EXCEPTIONS_SYSTEMEXCEPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../stdafx.h"
#include <string>

#include "Error.h"


/**
 * A C++ wrapper for GetLastError() in Win32 API.
 *
 * @see GetLastError
 */
class SystemException : public Bolota::Error {
protected:
	DWORD m_dwLastError;
	tstring m_strMessage;
	TCHAR *m_szLastErrorMessage;

	void Initialize(const TCHAR *szMessage);

public:
	SystemException(const TCHAR *szMessage) :
		Bolota::Error(szMessage) {
			m_szLastErrorMessage = NULL;
			Initialize(NULL);
	};
	SystemException() :
		Bolota::Error(_T("A system error occurred")) {
			m_szLastErrorMessage = NULL;
			Initialize(NULL);
	};
	virtual ~SystemException();

	void Throw() const override {
		m_message->TakeOwnership(_tcsdup(m_strMessage.c_str()));
		this->Error::Throw();
	};

	void RefreshMessage(const TCHAR *szMessage);
	const TCHAR* LastErrorMessage() const;
};

#endif // _BOLOTA_EXCEPTIONS_SYSTEMEXCEPTION_H
