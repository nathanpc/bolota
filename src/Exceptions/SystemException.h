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

#include <windows.h>
#include <stdexcept>
#include <string>
#if _MSC_VER <= 1200
	#include <newcpp.h>
#endif // _MSC_VER == 1200


/**
 * A C++ wrapper for GetLastError() in Win32 API.
 *
 * @see GetLastError
 */
class SystemException : public std::exception {
protected:
	DWORD m_dwLastError;
	std::string m_strMessage;
	char *m_szLastErrorMessage;

	void Initialize(const char *szMessage);

public:
	SystemException(const char *szMessage) :
		std::exception(szMessage) {
			m_szLastErrorMessage = NULL;
			Initialize(NULL);
	};
	SystemException() :
		std::exception("A system error occurred") {
			m_szLastErrorMessage = NULL;
			Initialize(NULL);
	};
	virtual ~SystemException();

	const char* what() const override {
		return m_strMessage.c_str();
	};

	void RefreshMessage(const char *szMessage);
	const char* LastErrorMessage() const;
};

#endif // _BOLOTA_EXCEPTIONS_SYSTEMEXCEPTION_H
