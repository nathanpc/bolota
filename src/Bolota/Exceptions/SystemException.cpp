/**
 * SystemException.cpp
 * An exception that is thrown in order to wrap a GetLastError() call.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "SystemException.h"

/**
 * Gets the last error and populates the exception.
 *
 * @param szMessage Optional. An error message to override the default one.
 */
void SystemException::Initialize(const TCHAR *szMessage) {
	// Get last error message.
	LPTSTR szLastErrorMsg;
	m_dwLastError = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL,
		m_dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&szLastErrorMsg, 0, NULL);

	// Free any previous error message if required.
	if (m_szLastErrorMessage)
		free(m_szLastErrorMessage);

	// Set the new error message and build up the full exception message.
	m_szLastErrorMessage = szLastErrorMsg;
	RefreshMessage(szMessage);
}

/**
 * Handles the proper deallocation of objects inside the exception.
 */
SystemException::~SystemException() {
	if (m_szLastErrorMessage) {
		free(m_szLastErrorMessage);
		m_szLastErrorMessage = NULL;
	}
}

/**
 * Updates the message to be displayed by the exception.
 *
 * @param szMessage Optional. An error message to override the default one.
 */
void SystemException::RefreshMessage(const TCHAR *szMessage) {
	m_strMessage = (szMessage != NULL) ? szMessage :
		m_message->GetNativeString();
	m_strMessage += _T(": ");
	m_strMessage += m_szLastErrorMessage;
}

/**
 * Gets the last error message.
 *
 * @return Last error message from the system.
 */
const TCHAR* SystemException::LastErrorMessage() const {
	return m_szLastErrorMessage;
}
