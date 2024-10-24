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
void SystemException::Initialize(const char *szMessage) {
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

#ifndef UNICODE
	m_szLastErrorMessage = szLastErrorMsg;
#else
	// Get required buffer size for the message conversion and allocate it.
	int nLen = WideCharToMultiByte(CP_OEMCP, 0, szLastErrorMsg, -1, NULL, 0,
		NULL, NULL);
	if (nLen == 0)
		goto failure;
	m_szLastErrorMessage = (char *)malloc(nLen * sizeof(char));

	// Perform the message conversion.
	nLen = WideCharToMultiByte(CP_OEMCP, 0, szLastErrorMsg, -1,
		m_szLastErrorMessage, nLen, NULL, NULL);
	if (nLen == 0) {
failure:
		if (m_szLastErrorMessage)
			free(m_szLastErrorMessage);
		RefreshMessage("Something happened and while throwing an exception we "
			"died trying to convert the Win32 message to MBCS");
		return;
	}
#endif // !UNICODE

	// Build up the full exception message.
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
void SystemException::RefreshMessage(const char *szMessage) {
	m_strMessage = (szMessage != NULL) ? szMessage : what();
	m_strMessage += ": ";
	m_strMessage += m_szLastErrorMessage;
}

/**
 * Gets the last error message.
 *
 * @return Last error message from the system.
 */
const char* SystemException::LastErrorMessage() const {
	return m_szLastErrorMessage;
}
