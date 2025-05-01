/**
 * SystemError.cpp
 * An error that is thrown in order to wrap a system error.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "SystemError.h"

#ifndef _WIN32
	#include <errno.h>

	/**
	 * Wrapper for Windows GetLastError() to map to UNIX errno.
	 */
	#define GetLastError() (errno)
#endif // !_WIN32

using namespace Bolota;

/**
 * Initializes the system error with a default message.
 */
SystemError::SystemError() : Error(_T("A system error occurred")) {
	m_szLastErrorMessage = NULL;
	Initialize(NULL, GetLastError());
}

/**
 * Initializes the system error with a message.
 *
 * @param szMessage Friendly message to go with the system error.
 */
SystemError::SystemError(const TCHAR *szMessage) : Error(szMessage) {
	m_szLastErrorMessage = NULL;
	Initialize(NULL, GetLastError());
}

#ifdef _WIN32
/**
 * Initializes the system error with a message.
 *
 * @param szMessage Friendly message to go with the system error.
 * @paeam dwError   System error code for FormatMessage.
 */
SystemError::SystemError(const TCHAR* szMessage, DWORD dwError) :
	Error(szMessage) {
	m_szLastErrorMessage = NULL;
	Initialize(NULL, dwError);
}

/**
 * Gets the last error and populates the exception.
 *
 * @param szMessage Optional. An error message to override the default one.
 * @param dwError   System error code for FormatMessage.
 */
void SystemError::Initialize(const TCHAR *szMessage, DWORD dwError) {
	// Get last error message.
	LPTSTR szLastErrorMsg;
	m_dwLastError = dwError;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL,
		m_dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&szLastErrorMsg, 0, NULL);

	// Free any previous error message if required.
	if (m_szLastErrorMessage)
		LocalFree(m_szLastErrorMessage);

	// Set the new error message and build up the full exception message.
	m_szLastErrorMessage = szLastErrorMsg;
	RefreshMessage(szMessage);
}
#else
/**
 * Gets the last error and populates the exception.
 *
 * @param szMessage Optional. An error message to override the default one.
 * @param iError    System error code from errno.
 */
void SystemError::Initialize(const TCHAR *szMessage, int iError) {
	// Get last error message.
	m_iError = iError;
	LPTSTR szLastErrorMsg = strdup(strerror(iError));

	// Free any previous error message if required.
	if (m_szLastErrorMessage)
#ifdef _WIN32
		LocalFree(m_szLastErrorMessage);
#else
		free(m_szLastErrorMessage);
#endif // _WIN32

	// Set the new error message and build up the full exception message.
	m_szLastErrorMessage = szLastErrorMsg;
	RefreshMessage(szMessage);
}
#endif // _WIN32

/**
 * Handles the proper deallocation of objects inside the exception.
 */
SystemError::~SystemError() {
	if (m_szLastErrorMessage) {
#ifdef _WIN32
		LocalFree(m_szLastErrorMessage);
#else
		free(m_szLastErrorMessage);
#endif // _WIN32
		m_szLastErrorMessage = NULL;
	}
}

/**
 * Updates the message to be displayed by the exception.
 *
 * @param szMessage Optional. An error message to override the default one.
 */
void SystemError::RefreshMessage(const TCHAR *szMessage) {
	// Build up the string.
	tstring strMessage = (szMessage != NULL) ? szMessage : m_message;
	strMessage += _T(": ");
	strMessage += m_szLastErrorMessage;

	// Substitute the error message string.
	free(m_message);
	m_message = _tcsdup(strMessage.c_str());
}

/**
 * Gets the last error message.
 *
 * @return Last error message from the system.
 */
const TCHAR* SystemError::LastErrorMessage() const {
	return m_szLastErrorMessage;
}
