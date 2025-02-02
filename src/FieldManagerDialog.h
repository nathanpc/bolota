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
#ifdef UNDER_CE
	HBITMAP m_hbIconPreview;
#endif // UNDER_CE

	// Controls
	HWND lblContext;
	HWND txtValue;
	HWND cmbType;
	HWND dtpTimestamp;
	HWND cmbFieldIcon;
	HWND btnOK;
	HWND btnAltOK;
	HWND btnCancel;
#ifdef UNDER_CE
	HWND lblIcon;
#endif // UNDER_CE

	// Actions
	void SetTitle(LPCTSTR szTitle);
	void SetContextLabel(LPCTSTR szContext);
	void SetButtons(LPCTSTR szAltOK, LPCTSTR szOK, LPCTSTR szCancel);
	LPTSTR GetContentText() const;

	// Events
	bool OnInit(HWND hDlg);
	void Close(INT_PTR nResult, bool bSelfDispose) override;
	INT_PTR OnTypeChange(int index);
	INT_PTR OnFieldIconComboDrawItem(LPDRAWITEMSTRUCT lpdis);
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