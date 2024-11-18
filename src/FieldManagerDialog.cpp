/**
 * FieldManagerDialog.cpp
 * A dialog to perform edit, create, append, prepend operations on a single
 * field.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "FieldManagerDialog.h"
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
 * @param hInst      Application's instance that this dialog belongs to.
 * @param hwndParent Parent window handle.
 * @param field      Field to be associated with this dialog.
 * @param context    Field providing context to the operation.
 */
FieldManagerDialog::FieldManagerDialog(HINSTANCE& hInst, HWND& hwndParent,
									   DialogType type, Field *field,
									   Field *context) :
	DialogWindow(hInst, hwndParent, IDD_FIELDMAN) {
	SetType(type);
	
	SetAlternativeSelected(false);
	m_field = field;
	m_context = context;
	m_fieldType = NULL;
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

	// Set the content of the editor fields.
	if (m_field->HasText())
		SetWindowText(txtValue, m_field->Text()->GetNativeString());
	if (m_field->Type() == BOLOTA_TYPE_DATE) {
		DateField *field = static_cast<DateField*>(m_field);
		DateTime_SetSystemtime(dtpTimestamp, GDT_VALID,
			(LPARAM)&field->ToSystemTime());
		m_stTimestamp = field->ToSystemTime();
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

	// Handle each type change.
	if (m_fieldType->code == BOLOTA_TYPE_TEXT) {
		MoveWindow(txtValue, rcEditorArea.left, rcEditorArea.top,
			rcEditorArea.right - rcEditorArea.left,
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(dtpTimestamp, SW_HIDE);
		ShowWindow(txtValue, SW_SHOW);
	} else if (m_fieldType->code == BOLOTA_TYPE_DATE) {
		MoveWindow(txtValue, rcDTP.right + CONTROL_SPACING, rcEditorArea.top,
			rcEditorArea.right - (rcDTP.right + CONTROL_SPACING),
			rcEditorArea.bottom - rcEditorArea.top, TRUE);
		ShowWindow(txtValue, SW_SHOW);
		ShowWindow(dtpTimestamp, SW_SHOW);
	}

	return TRUE;
}

/**
 * Event that occurs whenever the default OK button is pressed in the dialog.
 *
 * @return TRUE to close the dialog. FALSE to prevent it from closing.
 */
bool FieldManagerDialog::OnOK() {
	// Set the field's values.
	m_field->SetTextOwner(GetContentText());
	switch (m_field->Type()) {
	case BOLOTA_TYPE_TEXT:
		break;
	case BOLOTA_TYPE_DATE:
	{
		DateField *field = static_cast<DateField*>(m_field);
		field->SetTimestamp(&m_stTimestamp);
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
 * Sets up the field type combobox and selects the current field type.
 */
void FieldManagerDialog::SetupFieldTypeCombo() {
	uint8_t i = 0;

	// Add field types to the combobox.
	for (i = 0; i < Bolota::fieldTypesList.size(); ++i) {
		SendMessage(cmbType, CB_ADDSTRING, 0,
			(LPARAM)Bolota::fieldTypesList[i]->name->GetNativeString());
	}

	// Select our field type.
	for (i = 0; i < Bolota::fieldTypesList.size(); ++i) {
		if (m_field->Type() == Bolota::fieldTypesList[i]->code) {
			SendMessage(cmbType, CB_SETCURSEL, i, 0);
			OnTypeChange(i);
			break;
		}
	}
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
void FieldManagerDialog::SetButtons(LPCTSTR szAltOK, LPCTSTR szOK, LPCTSTR szCancel) {
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
					return OnTypeChange(SendMessage((HWND)lParam, CB_GETCURSEL,
						0, 0));
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
				if (((LPNMDATETIMECHANGE)lParam)->dwFlags == GDT_VALID)
					m_stTimestamp = ((LPNMDATETIMECHANGE)lParam)->st;
				break;
			}
			break;
	}

	// Pass the message to the default message handler.
	return DefaultDlgProc(hDlg, wMsg, wParam, lParam);
}
