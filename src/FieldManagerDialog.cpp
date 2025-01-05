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
	DialogWindow(hInst, hwndParent, IDD_FIELDMAN) {
	SetType(type);
	m_imlFieldIcons = imlFieldIcons;
	
	SetAlternativeSelected(false);
	m_field = field;
	m_context = context;
	m_fieldType = NULL;
	GetSystemTime(&m_stTimestamp);
	m_fiIndex = BOLOTA_ICON_BATTERY;
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
	
	// Get editor window position and dimensions.
	GetWindowRect(txtValue, &rcEditorArea);
	RectScreenToClient(&rcEditorArea, hDlg);
	RECT rcEdit = rcEditorArea;

	// Create timestamp Date Time Picker.
	dtpTimestamp = CreateWindowEx(0, DATETIMEPICK_CLASS, _T("DateTime"),
		WS_CHILD | WS_TABSTOP | DTS_RIGHTALIGN | DTS_APPCANPARSE,
		rcEdit.left, rcEdit.top, 150, rcEdit.bottom - rcEdit.top,
		hDlg, NULL, this->hInst, NULL);
	DateTime_SetFormat(dtpTimestamp, _T("yyyy'-'MM'-'dd HH':'mm':'ss"));
	DateTime_SetSystemtime(dtpTimestamp, GDT_VALID, (LPARAM)&m_stTimestamp);

	// Create field icon ComboBoxEx.
	cbeFieldIcon = CreateWindowEx(0, WC_COMBOBOXEX, NULL,
		WS_CHILD | WS_TABSTOP | CBS_DROPDOWN,
		rcEdit.left, rcEdit.top, 110, 150,
		hDlg, NULL, this->hInst, NULL);
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
		ComboBox_SetCurSel(cbeFieldIcon, m_fiIndex - 1);
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
		Close(IDCANCEL);
		return true;
	}

	return false;
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
	RECT rcCBE;
	GetWindowRect(cbeFieldIcon, &rcCBE);
	RectScreenToClient(&rcCBE, hDlg);

	// Handle each type change.
	if (m_fieldType->code == BOLOTA_TYPE_TEXT) {
		MoveWindow(txtValue, rcEditorArea.left, rcEditorArea.top,
			rcEditorArea.right - rcEditorArea.left,
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(dtpTimestamp, SW_HIDE);
		ShowWindow(cbeFieldIcon, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
	} else if (m_fieldType->code == BOLOTA_TYPE_DATE) {
		MoveWindow(txtValue, rcDTP.right + CONTROL_SPACING, rcEditorArea.top,
			rcEditorArea.right - (rcDTP.right + CONTROL_SPACING),
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(cbeFieldIcon, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
		ShowWindow(dtpTimestamp, SW_SHOW);
	} else if (m_fieldType->code == BOLOTA_TYPE_ICON) {
		MoveWindow(txtValue, rcCBE.right + CONTROL_SPACING, rcEditorArea.top,
			rcEditorArea.right - (rcCBE.right + CONTROL_SPACING),
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(dtpTimestamp, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
		ShowWindow(cbeFieldIcon, SW_SHOW);
	}

	return TRUE;
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
 * Sets up the field icon ComboBoxEx.
 */
void FieldManagerDialog::SetupFieldIconCombo() {
	// Add field icons to the ComboBox.
	for (UINT8 i = 0; i < FieldImageList::NumAvailableIcons(); ++i) {
		field_icon_t fi = IconField::IconList[i];
		UINT8 usFieldIconIndex = m_imlFieldIcons->IndexFromFieldIndex(fi);

		// Build up the item.
		COMBOBOXEXITEM cbei = {0};
		cbei.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE |
			CBEIF_LPARAM;
		cbei.iItem = -1;
		cbei.pszText = (LPTSTR)m_imlFieldIcons->LabelFromFieldIndex(fi);
		cbei.iImage = usFieldIconIndex;
		cbei.iSelectedImage = usFieldIconIndex;
		cbei.lParam = fi;

		// Add the item to the ComboBox.
		LRESULT lr = SendMessage(cbeFieldIcon, CBEM_INSERTITEM, 0,
			(LPARAM)&cbei);
		if (lr == -1) {
			ThrowError(new SystemError(EMSG("Failed to insert item into field ")
				_T("icons ComboBoxEx")));
			return;
		}
	}

	// Set the ComboBoxEx image list.
	SendMessage(cbeFieldIcon, CBEM_SETIMAGELIST, 0,
		(LPARAM)m_imlFieldIcons->Handle());

	// Select the default item for starters.
	ComboBox_SetCurSel(cbeFieldIcon, m_fiIndex - 1);
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
			case IDC_FM_BTNALTOK:
				if (OnAlternativeOK()) {
					Close(IDC_FM_BTNALTOK);
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
			case CBEN_ENDEDIT:
				// ComboBoxEx selection changed.
				if (((LPNMCBEENDEDIT)lParam)->iNewSelection != CB_ERR) {
					m_fiIndex = (field_icon_t)(((LPNMCBEENDEDIT)
						lParam)->iNewSelection + 1);
				}
				break;
			}
			break;
	}

	// Pass the message to the default message handler.
	return DefaultDlgProc(hDlg, wMsg, wParam, lParam);
}
