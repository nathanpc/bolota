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
#include "Components/FieldImageList.h"
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
	Bolota::Field **m_field;
	Bolota::Field *m_context;
	Bolota::FieldType *m_fieldType;
	bool m_bAlternative;
	SYSTEMTIME m_stTimestamp;
	FieldImageList *m_imlFieldIcons;
	field_icon_t m_fiIndex;
	RECT rcEditorArea;

	// Controls
	HWND lblContext;
	HWND txtValue;
	HWND cmbType;
	HWND dtpTimestamp;
	HWND cbeFieldIcon;
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
	INT_PTR OnTypeChange(int index);
	bool OnOK();
	bool OnAlternativeOK();
	bool OnCancel();

	// Setups
	void SetupFieldTypeCombo();
	void SetupFieldIconCombo();
	bool SetupEditControls();
	bool SetupAppendControls();
	bool SetupPrependControls();
	bool SetupCreateChildControls();

	// Setters
	void SetType(DialogType type);
	void SetAlternativeSelected(bool bAlternative);
	
	// Dialog Procedure
	INT_PTR CALLBACK DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
		LPARAM lParam);

public:
	// Constructors and destructors.
	FieldManagerDialog(HINSTANCE& hInst, HWND& hwndParent,
		FieldImageList *imlFieldIcons, DialogType type,
		Bolota::Field **field, Bolota::Field *context);

	// Getters
	DialogType Type() const;
	Bolota::Field* AssociatedField() const;
	bool AlternativeSelected() const;
};

#endif // _BOLOTA_FIELDMANAGERDIALOG_H