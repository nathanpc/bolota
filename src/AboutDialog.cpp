/**
 * AboutDialog.cpp
 * Application's about dialog.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "AboutDialog.h"

/**
 * Initializes the dialog window object.
 *
 * @param hInst      Application's instance that this dialog belongs to.
 * @param hwndParent Parent window handle.
 */
AboutDialog::AboutDialog(HINSTANCE& hInst, HWND& hwndParent) :
	DialogWindow(hInst, hwndParent, IDD_ABOUTBOX, true) {
	lblAppName = NULL;
	lblCopyright = NULL;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             Event Handlers                                |
 * |                                                                           |
 * +===========================================================================+
 */

 /**
  * Handles the dialog WM_INITDIALOG message.
  *
  * @param hDlg Dialog handle.
  *
  * @return TRUE if we handled the initialization or FALSE if we should pass the
  *         handling to the default dialog procedure.
  */
bool AboutDialog::OnInit(HWND hDlg) {
	// Get the handle of every useful control in the window.
	lblAppName = GetDlgItem(hDlg, IDC_AB_LBLAPPNAME);
	lblCopyright = GetDlgItem(hDlg, IDC_AB_LBLCOPYRIGHT);

	// Get application name.
	TCHAR szAppName[20];
	LoadString(hInst, IDS_APP_TITLE, szAppName, 20);

#ifndef UNDERCE
	// Get version data.
	VS_FIXEDFILEINFO* lpFileInfo;
	UINT uLen;
	HRSRC hrcInfo = FindResource(hInst, MAKEINTRESOURCE(VS_VERSION_INFO),
		RT_VERSION);
	if (hrcInfo == NULL) {
		ThrowError(EMSG("Failed to find version information resource"));
		return false;
	}
	DWORD dwSize = SizeofResource(hInst, hrcInfo);
	HGLOBAL hrcData = LoadResource(hInst, hrcInfo);
	if (hrcData == NULL) {
		ThrowError(EMSG("Failed to load version information resource"));
		return false;
	}
	LPVOID lpResource = LockResource(hrcData);
	LPVOID lpResourceCopy = LocalAlloc(LMEM_FIXED, dwSize);
	if (lpResourceCopy == NULL)
		return false;
	CopyMemory(lpResourceCopy, lpResource, dwSize);
	FreeResource(hrcData);
	VerQueryValue(lpResourceCopy, _T("\\"), (LPVOID*)&lpFileInfo, &uLen);

	// Populate application's name label with version data.
	TCHAR szBuffer[80];
	_stprintf(szBuffer, _T("%s v%u.%u.%u"), szAppName,
		HIWORD(lpFileInfo->dwProductVersionMS),
		LOWORD(lpFileInfo->dwProductVersionMS),
		HIWORD(lpFileInfo->dwProductVersionLS));
	SetWindowText(lblAppName, szBuffer);
#else
	// Populate application's name label.
	TCHAR szBuffer[80];
	SetWindowText(lblAppName, szAppName);
#endif // UNDERCE

	// Populate copyright label.
	LoadString(hInst, IDS_COPYRIGHT, szBuffer, 80);
	SetWindowText(lblCopyright, szBuffer);

	// Clean up version data.
	LocalFree(lpResourceCopy);
	lpResourceCopy = NULL;

	return false;
}

/**
 * Opens the application author's website.
 */
void AboutDialog::OnAuthorWebsite() {
	TCHAR szWebsite[100];
	LoadString(hInst, IDS_AUTHORWEBSITE, szWebsite, 100);
	ShellExecute(hwndParent, _T("open"), szWebsite, NULL, NULL, SW_SHOWNORMAL);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                            Dialog Procedure                               |
 * |                                                                           |
 * +===========================================================================+
 */

 /**
  * Dialog window procedure.
  *
  * @param hDlg   Dialog window handle.
  * @param wMsg   Message type.
  * @param wParam Message parameter.
  * @param lParam Message parameter.
  *
  * @return TRUE if the message was handled by the function, FALSE otherwise.
  *
  * @see DefaultDlgProc
  */
INT_PTR CALLBACK AboutDialog::DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
									  LPARAM lParam) {
	// Handle messages.
	switch (wMsg) {
	case WM_INITDIALOG:
		if (OnInit(hDlg))
			return TRUE;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_AB_BTNAUTHORWEB:
			OnAuthorWebsite();
			break;
		}
		break;
	}

	// Pass the message to the default message handler.
	return DefaultDlgProc(hDlg, wMsg, wParam, lParam);
}

