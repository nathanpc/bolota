/**
 * FieldManagerDialog.cpp
 * A dialog to perform edit, create, append, prepend operations on a single
 * field.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FieldManagerDialog.h"

#include <windowsx.h>

#include "Bolota/Errors/SystemError.h"
#include "Bolota/DateField.h"

// Spacing between controls.
#define CONTROL_SPACING 7

using namespace Bolota;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Initializes the dialog window object.
 *
 * @param hInst         Application's instance that this dialog belongs to.
 * @param hwndParent    Parent window handle.
 * @param imlFieldIcons Field icons ImageList manager.
 * @param field         Field to be associated with this dialog.
 * @param context       Field providing context to the operation.
 */
FieldManagerDialog::FieldManagerDialog(HINSTANCE& hInst, HWND& hwndParent,
									   FieldImageList *imlFieldIcons, DialogType type,
									   Field **field, Field *context) :
	DialogWindow(hInst, hwndParent, IDD_FIELDMAN, false) {
	SetType(type);
	m_imlFieldIcons = imlFieldIcons;
	
	SetAlternativeSelected(false);
	m_field = field;
	m_context = context;
	m_fieldType = NULL;
	GetSystemTime(&m_stTimestamp);
	m_fiIndex = BOLOTA_ICON_BATTERY;

#ifdef UNDER_CE
	m_hbIconPreview = NULL;
#endif // UNDER_CE
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
bool FieldManagerDialog::OnInit(HWND hDlg) {
	// Get the handle of every useful control in the window.
	lblContext = GetDlgItem(hDlg, IDC_FM_LBLCONTEXT);
	txtValue = GetDlgItem(hDlg, IDC_FM_EDTCONTENT);
	cmbType = GetDlgItem(hDlg, IDC_FM_CMBTYPE);
	btnAltOK = GetDlgItem(hDlg, IDC_FM_BTNALTOK);
	btnOK = GetDlgItem(hDlg, IDOK);
	btnCancel = GetDlgItem(hDlg, IDCANCEL);

	// Ensure we are centered.
	CenterInParent();

	// Get client area.
	RECT rcClient;
	GetClientRect(this->hwndParent, &rcClient);
	
	// Get editor window position and dimensions.
	GetWindowRect(txtValue, &rcEditorArea);
	RectScreenToClient(&rcEditorArea, hDlg);
	RECT rcEdit = rcEditorArea;

	// Ensure field type ComboBox has enough space to expand its list.
	RECT rcType;
	GetWindowRect(cmbType, &rcType);
	RectScreenToClient(&rcType, hDlg);
	SetWindowPos(cmbType, NULL, rcType.left, rcType.top,
		rcType.right - rcType.left, rcClient.bottom - rcType.top,
		SWP_NOZORDER);

	// Create timestamp Date Time Picker.
	dtpTimestamp = CreateWindowEx(0, DATETIMEPICK_CLASS, _T("DateTime"),
		WS_CHILD | WS_TABSTOP | DTS_RIGHTALIGN | DTS_APPCANPARSE,
		rcEdit.left, rcEdit.top, 150, rcEdit.bottom - rcEdit.top,
		hDlg, (HMENU)IDC_FM_DTPTIMESTAMP, this->hInst, NULL);
	DateTime_SetFormat(dtpTimestamp, _T("yyyy'-'MM'-'dd HH':'mm':'ss"));
	DateTime_SetSystemtime(dtpTimestamp, GDT_VALID, (LPARAM)&m_stTimestamp);

	// Create field icon ComboBox.
#ifndef UNDER_CE
	cmbFieldIcon = CreateWindow(WC_COMBOBOX, _T(""), WS_CHILD | WS_TABSTOP |
		WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED,
		rcEdit.left, rcEdit.top, 105, rcClient.bottom - rcEdit.top, hDlg,
		(HMENU)IDC_FM_CMBFIELDICON, this->hInst, NULL);
#else
	RECT rcComboIcon = rcEdit;
	rcComboIcon.right = 105 - (CONTROL_SPACING * 2) - 16;
	rcComboIcon.bottom = rcClient.bottom - rcEdit.top;
	cmbFieldIcon = CreateWindow(_T("COMBOBOX"), NULL, WS_CHILD | WS_TABSTOP |
		WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS, rcComboIcon.left,
		rcComboIcon.top, rcComboIcon.right, rcComboIcon.bottom, hDlg,
		(HMENU)IDC_FM_CMBFIELDICON, this->hInst, NULL);

	// Create field icon preview static control.
	RECT rcIcon = rcComboIcon;
	rcIcon.right = 16;
	rcIcon.bottom = rcEdit.bottom - rcEdit.top;
	rcIcon.left = rcComboIcon.left + rcComboIcon.right + CONTROL_SPACING;
	lblIcon = CreateWindow(_T("STATIC"), NULL, WS_CHILD | SS_CENTERIMAGE |
		SS_BITMAP, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, hDlg,
		(HMENU)IDC_FM_LBLICON, this->hInst, NULL);
	OnFieldIconComboDrawItem(NULL);
#endif // !UNDER_CE
	SetupFieldIconCombo();
	if (BolotaHasError) {
		MsgBoxBolotaError(hwndParent, _T("Failed to set up icon ComboBox"));
		return false;
	}

	// Set the content of the editor fields.
	if (AssociatedField()->HasText())
		SetWindowText(txtValue, AssociatedField()->Text()->GetNativeString());
	if (AssociatedField()->Type() == BOLOTA_TYPE_DATE) {
		// Set timestamp of Date Time Picker.
		DateField *field = static_cast<DateField*>(AssociatedField());
		SYSTEMTIME st = field->ToSystemTime();
		DateTime_SetSystemtime(dtpTimestamp, GDT_VALID, (LPARAM)&st);
		m_stTimestamp = field->ToSystemTime();
	} else if (AssociatedField()->Type() == BOLOTA_TYPE_ICON) {
		// Set field icon in ComboBox.
		IconField *field = static_cast<IconField*>(AssociatedField());
		m_fiIndex = field->IconIndex();
		ComboBox_SetCurSel(cmbFieldIcon, m_fiIndex - 1);
#ifdef UNDER_CE
		OnFieldIconComboDrawItem(NULL);
#endif // UNDER_CE
	}

	// Set up the type ComboBox and switch to the correct layout for the dialog.
	SetupFieldTypeCombo();

	// Decide which setup to use.
	switch (m_type) {
	case EditField:
		return SetupEditControls();
	case AppendField:
		return SetupAppendControls();
	case PrependField:
		return SetupPrependControls();
	case NewChildField:
		return SetupCreateChildControls();
	default:
		MsgBoxError(hDlg, _T("Unknown dialog type"), _T("Couldn't setup ")
			_T("the Field Manager dialog box for the requested type."));
		DialogWindow::Close(IDCANCEL);
		return true;
	}

	return false;
}

/**
 * Closes the dialog window.
 *
 * @remark Calls the parent's Close method.
 *
 * @param nResult      Return value of the dialog.
 * @param bSelfDispose Are we allowed to self dispose if asked to?
 */
void FieldManagerDialog::Close(INT_PTR nResult, bool bSelfDispose) {
#ifdef UNDER_CE
	// Dispose of the preview icon and control.
	if (m_hbIconPreview)
		DeleteObject(m_hbIconPreview);
	m_hbIconPreview = NULL;
#endif // UNDER_CE

	// Continue closing.
	DialogWindow::Close(nResult, bSelfDispose);
}

/**
 * Handles the change of the field type combobox selected item.
 *
 * @param index Index of the newly selected field type.
 *
 * @return TRUE if the event was handled.
 */
INT_PTR FieldManagerDialog::OnTypeChange(int index) {
	m_fieldType = Bolota::fieldTypesList[index];
	
	// Get date time picker window position and dimensions.
	RECT rcDTP;
	GetWindowRect(dtpTimestamp, &rcDTP);
	RectScreenToClient(&rcDTP, hDlg);
	
	// Get icon combobox window position and dimensions.
	RECT rcCMB;
	GetWindowRect(cmbFieldIcon, &rcCMB);
	RectScreenToClient(&rcCMB, hDlg);

	// Handle each type change.
	if (m_fieldType->code == BOLOTA_TYPE_TEXT) {
		MoveWindow(txtValue, rcEditorArea.left, rcEditorArea.top,
			rcEditorArea.right - rcEditorArea.left,
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(dtpTimestamp, SW_HIDE);
		ShowWindow(cmbFieldIcon, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
#ifdef UNDER_CE
		ShowWindow(lblIcon, SW_HIDE);
#endif // UNDER_CE
	} else if (m_fieldType->code == BOLOTA_TYPE_DATE) {
		MoveWindow(txtValue, rcDTP.right + CONTROL_SPACING, rcEditorArea.top,
			rcEditorArea.right - (rcDTP.right + CONTROL_SPACING),
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(cmbFieldIcon, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
		ShowWindow(dtpTimestamp, SW_SHOW);
#ifdef UNDER_CE
		ShowWindow(lblIcon, SW_HIDE);
#endif // UNDER_CE
	} else if (m_fieldType->code == BOLOTA_TYPE_ICON) {
		LONG lLeft = rcCMB.right + CONTROL_SPACING;
#ifdef UNDER_CE
		RECT rcIcon;
		GetWindowRect(lblIcon, &rcIcon);
		RectScreenToClient(&rcIcon, hDlg);
		lLeft = rcIcon.right + CONTROL_SPACING;
#endif // UNDER_CE

		MoveWindow(txtValue, lLeft, rcEditorArea.top, rcEditorArea.right -
			lLeft, rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(dtpTimestamp, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
		ShowWindow(cmbFieldIcon, SW_SHOW);
#ifdef UNDER_CE
		ShowWindow(lblIcon, SW_SHOW);
#endif // UNDER_CE
	}

	return TRUE;
}

/**
 * Handles the owner-drawn WM_DRAWITEM message of the field icon ComboBox.
 * 
 * @param lpdis Structure containing everything necessary for drawing special
 *              things.
 *
 * @return TRUE if the event was handled.
 */
INT_PTR FieldManagerDialog::OnFieldIconComboDrawItem(LPDRAWITEMSTRUCT lpdis) {
#ifndef UNDER_CE
	// Stupidity check.
	if (lpdis == NULL) {
		ThrowError(EMSG("OnFieldIconComboDrawItem lpdis is NULL"));
		return FALSE;
	}

	// Ignore empty items.
	if (lpdis->itemID == -1)
		return FALSE;

	// Get default colors for the item (selected or unselected).
	COLORREF clrForeground = SetTextColor(lpdis->hDC, GetSysColor(
		(lpdis->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT :
		COLOR_WINDOWTEXT));
	COLORREF clrBackground = SetBkColor(lpdis->hDC, GetSysColor(
		(lpdis->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHT :
		COLOR_WINDOW));

	// Calculate the vertical and horizontal position.
	TEXTMETRIC tm;
	GetTextMetrics(lpdis->hDC, &tm);
	int y = (lpdis->rcItem.bottom + lpdis->rcItem.top -
		tm.tmHeight) / 2;
	int x = LOWORD(GetDialogBaseUnits()) / 4;

	// Get label text to be displayed.
	int iTextLen = ComboBox_GetLBTextLen(lpdis->hwndItem,
		lpdis->itemID);
	LPTSTR szLabel = (LPTSTR)malloc((iTextLen + 1) * sizeof(TCHAR));
	ComboBox_GetLBText(lpdis->hwndItem, lpdis->itemID, szLabel);

	// Draw the text label and free up its buffer.
	ExtTextOut(lpdis->hDC, m_imlFieldIcons->IconSize() + 2 * x, y, ETO_CLIPPED |
		ETO_OPAQUE, &lpdis->rcItem, szLabel, (UINT)iTextLen, NULL);
	free(szLabel);
	szLabel = NULL;

	// Restore the previous colors.
	SetTextColor(lpdis->hDC, clrForeground);
	SetBkColor(lpdis->hDC, clrBackground);

	// Draw the icon image.
	UINT8 usFieldIconIndex = m_imlFieldIcons->IndexFromFieldIndex(
		(field_icon_t)lpdis->itemData);
	if (!ImageList_Draw(m_imlFieldIcons->Handle(), usFieldIconIndex, lpdis->hDC,
			x, lpdis->rcItem.top + 1, ILD_TRANSPARENT)) {
		return FALSE;
	}

	// If the item has the focus, draw the focus rectangle.
	if (lpdis->itemState & ODS_FOCUS)
		DrawFocusRect(lpdis->hDC, &lpdis->rcItem);

	return TRUE;
#else
	// Set the field icon preview.
	if (m_hbIconPreview)
		DeleteObject(m_hbIconPreview);

	// Get some device contexts for the operation.
	HDC hdcScreen = GetWindowDC(NULL);
	HDC hdcDest = CreateCompatibleDC(hdcScreen);

	// Get some objects together.
	RECT rc = {0};
	rc.bottom = m_imlFieldIcons->IconSize();
	rc.right = m_imlFieldIcons->IconSize();
	m_hbIconPreview = CreateCompatibleBitmap(hdcScreen, rc.right, rc.bottom);
	HGDIOBJ hgdiOld = SelectObject(hdcDest, m_hbIconPreview);

	// Set the background and draw the icon from the ImageList.
	FillRect(hdcDest, &rc, GetSysColorBrush(COLOR_MENU));
	ImageList_Draw(m_imlFieldIcons->Handle(),
		m_imlFieldIcons->IndexFromFieldIndex(m_fiIndex),
		hdcDest, 0, 0, ILD_TRANSPARENT);

	// Clean up.
	ReleaseDC(NULL, (HDC)hgdiOld);
	DeleteDC(hdcDest);

	// Set the bitmap onto the control.
	SendMessage(lblIcon, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_hbIconPreview);

	return TRUE;
#endif // !UNDER_CE
}

/**
 * Event that occurs whenever the default OK button is pressed in the dialog.
 *
 * @return TRUE to close the dialog. FALSE to prevent it from closing.
 */
bool FieldManagerDialog::OnOK() {
	// Change field type if needed.
	if (AssociatedField()->Type() != m_fieldType->code) {
		// Create the new field.
		Field *old = AssociatedField();
		switch (m_fieldType->code) {
		case BOLOTA_TYPE_TEXT:
			*m_field = new TextField();
			break;
		case BOLOTA_TYPE_DATE:
			*m_field = new DateField();
			break;
		case BOLOTA_TYPE_ICON:
			*m_field = new IconField();
			break;
		default:
			MsgBoxError(hDlg, _T("Unknown field type"),
				_T("This type of field conversion wasn't yet implemented."));
			goto skipreplace;
		}
		
		// Replace the actual field.
		(*m_field)->Copy(old, true);
		delete old;
	}

skipreplace:
	// Set the field's values.
	AssociatedField()->SetTextOwner(GetContentText());
	switch (AssociatedField()->Type()) {
	case BOLOTA_TYPE_TEXT:
		break;
	case BOLOTA_TYPE_DATE: {
		DateField *field = static_cast<DateField*>(AssociatedField());
		field->SetTimestamp(&m_stTimestamp);
		break;
	}
	case BOLOTA_TYPE_ICON: {
		IconField *field = static_cast<IconField*>(AssociatedField());
		field->SetIconIndex(m_fiIndex);
		break;
	}
	default:
		MsgBoxError(hDlg, _T("Unknown field type"),
			_T("This type of field saving wasn't yet implemented."));
	}

	// Perform specific operations if needed.
	switch (m_type) {
	case EditField:
	case AppendField:
	case PrependField:
	case NewChildField:
		return true;
	default:
		MsgBoxError(hDlg, _T("Unknown dialog type"),
			_T("This type of operation wasn't yet implemented."));
	}

	return false;
}

/**
 * Event that occurs whenever the alternative OK button is pressed in the
 * dialog.
 *
 * @return TRUE if we properly handled the event.
 */
bool FieldManagerDialog::OnAlternativeOK() {
	// Flag that the alternative OK was pressed.
	SetAlternativeSelected(true);

	// Perform specific operations if needed.
	switch (m_type) {
	case AppendField:
		// Prepend
		SetType(DialogType::PrependField);
		break;
	case PrependField:
		// Append
		SetType(DialogType::AppendField);
		break;
	default:
		MsgBoxError(hDlg, _T("Unknown type"),
			_T("This type of operation wasn't yet implemented."));
		return true;
	}

	// Perform the equivalent of an OK action.
	OnOK();

	return true;
}

/**
 * Event that occurs whenever the default cancel button is pressed in the
 * dialog.
 *
 * @return TRUE to close the dialog. FALSE to prevent it from closing.
 */
bool FieldManagerDialog::OnCancel() {
	return true;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             Controls Setup                                |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Sets up the field type ComboBox and selects the current field type.
 */
void FieldManagerDialog::SetupFieldTypeCombo() {
	UINT8 i = 0;

	// Add field types to the combobox.
	for (i = 0; i < Bolota::fieldTypesList.size(); ++i) {
		ComboBox_AddString(cmbType,
			Bolota::fieldTypesList[i]->name->GetNativeString());
	}

	// Select our field type.
	for (i = 0; i < Bolota::fieldTypesList.size(); ++i) {
		if (AssociatedField()->Type() == Bolota::fieldTypesList[i]->code) {
			ComboBox_SetCurSel(cmbType, i);
			OnTypeChange(i);
			break;
		}
	}
}

/**
 * Sets up the field icon ComboBox.
 */
void FieldManagerDialog::SetupFieldIconCombo() {
#ifndef UNDER_CE
	// Set a nicer font for the control.
	SendMessage(cmbFieldIcon, WM_SETFONT,
		(WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
#endif // !UNDER_CE

	// Add field icons to the ComboBox.
	for (UINT8 i = 0; i < FieldImageList::NumAvailableIcons(); ++i) {
		field_icon_t fi = IconField::IconList[i];

		// Add string to ComboBox.
		DWORD dwIndex = ComboBox_AddString(cmbFieldIcon,
			m_imlFieldIcons->LabelFromFieldIndex(fi));
		if (dwIndex == CB_ERR) {
			ThrowError(new SystemError(EMSG("Failed to insert string into ")
				_T("field icons ComboBox")));
			return;
		}

		// Set item data to the last added item in the ComboBox.
		DWORD dwIndexData = ComboBox_SetItemData(cmbFieldIcon, dwIndex,
			(LPARAM)fi);
		if (dwIndexData == CB_ERR) {
			ThrowError(new SystemError(EMSG("Failed to set item data in field ")
				_T("icons ComboBox")));
			return;
		}
	}

	// Select the default item for starters.
	ComboBox_SetCurSel(cmbFieldIcon, m_fiIndex - 1);
}

/**
 * Sets up the dialog for editing an existing field.
 *
 * @return TRUE if everything worked.
 */
bool FieldManagerDialog::SetupEditControls() {
	SetTitle(_T("Edit Field"));
	SetContextLabel(m_context->Text()->GetNativeString());
	SetButtons(NULL, _T("Save"), _T("Cancel"));
	return true;
}

/**
 * Sets up the dialog for appending to an existing field.
 *
 * @return TRUE if everything worked.
 */
bool FieldManagerDialog::SetupAppendControls() {
	SetTitle(_T("Appending Field"));
	SetContextLabel(m_context->Text()->GetNativeString());
	SetButtons(_T("Prepend"), _T("Append"), _T("Cancel"));
	return true;
}

/**
 * Sets up the dialog for prepending to an existing field.
 *
 * @return TRUE if everything worked.
 */
bool FieldManagerDialog::SetupPrependControls() {
	SetTitle(_T("Prepending Field"));
	SetContextLabel(m_context->Text()->GetNativeString());
	SetButtons(_T("Append"), _T("Prepend"), _T("Cancel"));
	return true;
}

/**
 * Sets up the dialog for creating a child of an existing field.
 *
 * @return TRUE if everything worked.
 */
bool FieldManagerDialog::SetupCreateChildControls() {
	SetTitle(_T("Creating Child Field"));
	SetContextLabel(m_context->Text()->GetNativeString());
	SetButtons(NULL, _T("Create"), _T("Cancel"));
	return true;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                Actions                                    |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the content of the edit field in the dialog.
 *
 * @warning This method allocates memory dynamically.
 *
 * @return Contents of the edit field.
 */
LPTSTR FieldManagerDialog::GetContentText() const {
	// Get the length of the required string.
	int nLen = GetWindowTextLength(txtValue);
	if (nLen == 0)
		return NULL;
	nLen++;  // To include NUL terminator.

	// Allocate the memory required for the string.
	LPTSTR szText = (LPTSTR)malloc(nLen * sizeof(TCHAR));
	if (szText == NULL)
		return NULL;

	// Get the text from the control.
	GetWindowText(txtValue, szText, nLen);

	return szText;
}

/**
 * Sets the title of the dialog box.
 *
 * @param szTitle Title of the dialog box.
 */
void FieldManagerDialog::SetTitle(LPCTSTR szTitle) {
	SetWindowText(hDlg, szTitle);
}

/**
 * Sets the content of the context label in the dialog box.
 *
 * @param szContext Content of the context label.
 */
void FieldManagerDialog::SetContextLabel(LPCTSTR szContext) {
	SetWindowText(lblContext, szContext);
}

/**
 * Sets the captions of the dialog buttons accordingly.
 *
 * @param szAltOK  Caption of the alternative OK button. Use NULL to hide it.
 * @param szOK     Caption of the default OK button. Use NULL to hide it.
 * @param szCancel Caption of the default cancel button. Use NULL to hide it.
 */
void FieldManagerDialog::SetButtons(LPCTSTR szAltOK, LPCTSTR szOK,
									LPCTSTR szCancel) {
	if (szAltOK != NULL) {
		SetWindowText(btnAltOK, szAltOK);
	} else {
		ShowWindow(btnAltOK, SW_HIDE);
	}

	if (szOK != NULL) {
		SetWindowText(btnOK, szOK);
	} else {
		ShowWindow(btnOK, SW_HIDE);
	}

	if (szCancel != NULL) {
		SetWindowText(btnCancel, szCancel);
	} else {
		ShowWindow(btnCancel, SW_HIDE);
	}
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the type of the operation performed in the dialog. This may have changed
 * depending if the user clicked the Alternative OK button.
 *
 * @return Updated type of operation performed in the dialog box.
 */
FieldManagerDialog::DialogType FieldManagerDialog::Type() const {
	return m_type;
}

/**
 * Sets the type of the operation being performed in the dialog.
 *
 * @param type Type of operation being performed.
 */
void FieldManagerDialog::SetType(FieldManagerDialog::DialogType type) {
	m_type = type;
}

/**
 * Gets the field object that is being edited by this dialog.
 *
 * @return Field object associated with this dialog.
 */
Field* FieldManagerDialog::AssociatedField() const {
	return *m_field;
}

/**
 * Gets the flag that defines if the alternative OK button was selected.
 *
 * @return TRUE if the alternative OK was selected.
 */
bool FieldManagerDialog::AlternativeSelected() const {
	return m_bAlternative;
}

/**
 * Sets the flag that defines if the alternative OK button was selected.
 *
 * @param bAlternative Has the alternative OK button been pressed?
 */
void FieldManagerDialog::SetAlternativeSelected(bool bAlternative) {
	m_bAlternative = bAlternative;
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
INT_PTR CALLBACK FieldManagerDialog::DlgProc(HWND hDlg, UINT wMsg,
											 WPARAM wParam, LPARAM lParam) {
	// Handle messages.
	switch (wMsg) {
		case WM_INITDIALOG:
			if (OnInit(hDlg))
				return TRUE;
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDC_FM_CMBTYPE:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					return OnTypeChange(ComboBox_GetCurSel((HWND)lParam));
				}
				break;
			case IDC_FM_CMBFIELDICON:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					m_fiIndex = (field_icon_t)ComboBox_GetItemData((HWND)lParam,
						ComboBox_GetCurSel((HWND)lParam));

#ifdef UNDER_CE
					// Set the field icon preview.
					OnFieldIconComboDrawItem(NULL);
#endif // UNDER_CE
				}
				break;
			case IDC_FM_BTNALTOK:
				if (OnAlternativeOK()) {
					DialogWindow::Close(IDC_FM_BTNALTOK);
					return TRUE;
				}
			case IDOK:
				if (!OnOK())
					return FALSE;
				break;
			case IDCANCEL:
				if (!OnCancel())
					return FALSE;
				break;
			}
			break;
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code) {
			case DTN_DATETIMECHANGE:
				// Date Time Picker value changed.
				if (((LPNMDATETIMECHANGE)lParam)->dwFlags == GDT_VALID)
					m_stTimestamp = ((LPNMDATETIMECHANGE)lParam)->st;
				break;
			}
			break;
		case WM_MEASUREITEM:
			if (LOWORD(wParam) == IDC_FM_CMBFIELDICON) {
				// Set the height of the items in the field icon ComboBox.
				LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
				if (lpmis->itemHeight < (UINT)(m_imlFieldIcons->IconSize() + 2))
					lpmis->itemHeight = m_imlFieldIcons->IconSize() + 2;
			}
			break;
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_FM_CMBFIELDICON) {
				return OnFieldIconComboDrawItem((LPDRAWITEMSTRUCT)lParam);
			}
			break;
	}

	// Pass the message to the default message handler.
	return DefaultDlgProc(hDlg, wMsg, wParam, lParam);
}
