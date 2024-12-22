/**
 * MsgBoxes.cpp
 * Some utility functions to work with Message Boxes more easily and write less
 * boilerplate for something that is so simple.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "MsgBoxes.h"

#include "../stdafx.h"
#include "../Bolota/Errors/Error.h"

/**
 * Generic message box.
 * 
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 * @param uType      Flags that control the buttons and the icon of the message box.
 * @param szTitle    Title of the message box dialog window.
 * @param szText     Descriptive text of the dialog box.
 * 
 * @return ID of the button that was clicked by the user.
 */
int MsgBox(HWND hwndParent, UINT uType, LPCTSTR szTitle, LPCTSTR szText) {
	return MessageBox(hwndParent, szText, szTitle, uType);
}

/**
 * Information message box.
 *
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 * @param szTitle    Title of the message box dialog window.
 * @param szText     Descriptive text of the dialog box.
 *
 * @return ID of the button that was clicked by the user.
 */
int MsgBoxInfo(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText) {
	return MessageBox(hwndParent, szText, szTitle, MB_OK | MB_ICONINFORMATION);
}

/**
 * Warning message box.
 *
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 * @param szTitle    Title of the message box dialog window.
 * @param szText     Descriptive text of the dialog box.
 *
 * @return ID of the button that was clicked by the user.
 */
int MsgBoxWarning(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText) {
	return MessageBox(hwndParent, szText, szTitle, MB_OK | MB_ICONWARNING);
}

/**
 * Error message box.
 * 
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 * @param szTitle    Title of the message box dialog window.
 * @param szText     Descriptive text of the dialog box.
 *
 * @return ID of the button that was clicked by the user.
 */
int MsgBoxError(HWND hwndParent, LPCTSTR szTitle, LPCTSTR szText) {
	return MessageBox(hwndParent, szText, szTitle, MB_OK | MB_ICONERROR);
}

/**
 * Win32 last error message box.
 *
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 *
 * @return ID of the button that was clicked by the user or 0 if no error was
 *         reported.
 */
int MsgBoxLastError(HWND hwndParent) {
	DWORD dwError;
	LPTSTR szError;
	int nRet;

	// Get the last error code.
	if ((dwError = GetLastError()) == 0)
		return 0;

	// Get the detailed description of the error.
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szError, 0, NULL);

	// Show the message box and clean up afterwards.
	nRet = MsgBoxError(hwndParent, _T("Win32 API Error"), szError);
	LocalFree(szError);

	return nRet;
}

/**
 * Bolota error message box.
 *
 * @param hwndParent Parent window's handle or NULL if it doesn't have one.
 * @param szTitle    Title of the message box dialog window.
 *
 * @return ID of the button that was clicked by the user.
 */
int MsgBoxBolotaError(HWND hwndParent, LPCTSTR szTitle) {
	// Build up error message.
	tstring strMessage(Bolota::ErrorStack->Message());
	Bolota::Error *err = Bolota::ErrorStack->Pop();
	if (err != NULL)
		strMessage += _T("\r\nError Stack:\r\n");
	while (err != NULL) {
		strMessage += err->Message();
		strMessage += _T("\r\n");
		err = err->Pop();
	}

	// Show the error message box.
	return MsgBoxError(hwndParent, szTitle, strMessage.c_str());
}
