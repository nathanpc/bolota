/**
 * FieldManagerDialog.h
 * A dialog to perform edit, create, append, prepend operations on a single
 * field.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_FIELDMANAGERDIALOG_H
#define _BOLOTA_FIELDMANAGERDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "Utilities/DialogWindow.h"
#include "Bolota/Field.h"

/**
 * A dialog to perform edit, create, append, prepend operations on a single
 * field.
 */
class FieldManagerDialog : public DialogWindow {
public:
	enum DialogType {
		EditField,
		AppendField,
		PrependField,
		NewChildField
	};

protected:
	// Properties
	DialogType m_type;
	Bolota::Field *m_field;
	bool m_bAlternative;

	// Controls
	HWND lblContext;
	HWND txtValue;
	HWND btnOK;
	HWND btnAltOK;
	HWND btnCancel;

	// Actions
	void SetTitle(LPCTSTR szTitle);
	void SetContextLabel(LPCTSTR szContext);
	void SetButtons(LPCTSTR szAltOK, LPCTSTR szOK, LPCTSTR szCancel);
	LPTSTR GetContentText() const;

	// Events
	bool OnInit(HWND hDlg);
	bool OnOK();
	bool OnAlternativeOK();
	bool OnCancel();

	// Setups
	bool SetupEditControls();
	bool SetupAppendControls();
	bool SetupPrependControls();

	// Setters
	void SetType(DialogType type);
	void SetAlternativeSelected(bool bAlternative);
	
	// Dialog Procedure
	INT_PTR CALLBACK DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
		LPARAM lParam);

public:
	// Constructors and destructors.
	FieldManagerDialog(HINSTANCE& hInst, HWND& hwndParent, DialogType type,
		Bolota::Field *field);

	// Getters
	DialogType Type() const;
	bool AlternativeSelected() const;
};

#endif // _BOLOTA_FIELDMANAGERDIALOG_H