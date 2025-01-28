/**
 * AboutDialog.h
 * Application's about dialog.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _RODENT_ABOUTDIALOG_H
#define _RODENT_ABOUTDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Utilities/DialogWindow.h"

/**
 * Application's about dialog.
 */
class AboutDialog : public DialogWindow {
private:
	HWND lblAppName;
	HWND lblCopyright;

	// Events
	bool OnInit(HWND hDlg);
	void OnAuthorWebsite();

	// Dialog Procedure
	INT_PTR CALLBACK DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
		LPARAM lParam);

public:
	AboutDialog(HINSTANCE& hInst, HWND& hwndParent);
};

#endif // _RODENT_ABOUTDIALOG_H
