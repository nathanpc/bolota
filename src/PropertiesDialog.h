/**
 * PropertiesDialog.h
 * A dialog to manage a Bolota document's properties.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_PROPERTIESDIALOG_H
#define _BOLOTA_PROPERTIESDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Utilities/DialogWindow.h"
#include "Bolota/Document.h"

/**
 * A dialog to manage a Bolota document's properties.
 */
class PropertiesDialog : public DialogWindow {
protected:
	// Properties
	Bolota::Document *m_doc;

	// Controls
	HWND txtTitle;
	HWND txtSubTitle;
	HWND dtpDate;
	HWND btnOK;
	HWND btnCancel;
	
	// Events
	bool OnInit(HWND hDlg);
	bool OnOK();
	bool OnCancel();

	// Helpers
	LPTSTR GetFieldText(HWND hwndEdit) const;
	
	// Dialog Procedure
	INT_PTR CALLBACK DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
		LPARAM lParam);

public:
	PropertiesDialog(HINSTANCE& hInst, HWND& hwndParent, Bolota::Document *doc);
};

#endif // _BOLOTA_PROPERTIESDIALOG_H
