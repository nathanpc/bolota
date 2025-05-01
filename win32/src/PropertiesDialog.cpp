/**
 * PropertiesDialog.cpp
 * A dialog to manage a Bolota document's properties.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "PropertiesDialog.h"

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
 * @param doc        Bolota document to be edited.
 */
PropertiesDialog::PropertiesDialog(HINSTANCE& hInst, HWND& hwndParent,
								   Bolota::Document *doc) :
	DialogWindow(hInst, hwndParent, IDD_DOCPROPS, true) {
	m_doc = doc;
	memset(&m_st, 0, sizeof(SYSTEMTIME));

	txtTitle = NULL;
	txtSubTitle = NULL;
	dtpDate = NULL;
	btnOK = NULL;
	btnCancel = NULL;
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
bool PropertiesDialog::OnInit(HWND hDlg) {
	// Get the handle of every useful control in the window.
	txtTitle = GetDlgItem(hDlg, IDC_DP_EDTTITLE);
	txtSubTitle = GetDlgItem(hDlg, IDC_DP_EDTSUBTITLE);
	dtpDate = GetDlgItem(hDlg, IDC_DP_DTPTIMESTAMP);
	btnOK = GetDlgItem(hDlg, IDOK);
	btnCancel = GetDlgItem(hDlg, IDCANCEL);

	// Ensure the Date Time Picker shows the timestamp in ISO8601.
	DateTime_SetFormat(dtpDate, _T("yyyy'-'MM'-'dd HH':'mm':'ss"));

	// Set the contents of the fields according to the document properties.
	SetWindowText(txtTitle, m_doc->Title()->Text()->GetNativeString());
	SetWindowText(txtSubTitle, m_doc->SubTitle()->Text()->GetNativeString());
	SYSTEMTIME st = m_doc->Date()->ToSystemTime();
	DateTime_SetSystemtime(dtpDate, GDT_VALID, &st);

	return false;
}

/**
 * Event that occurs whenever the default OK button is pressed in the dialog.
 *
 * @return TRUE to close the dialog. FALSE to prevent it from closing.
 */
bool PropertiesDialog::OnOK() {
	// Update document properties.
	m_doc->SetTitle(GetFieldText(txtTitle));
	m_doc->SetSubTitle(GetFieldText(txtSubTitle));
	if (m_st.wYear != 0)
		m_doc->SetDate(&m_st);

	return true;
}

/**
 * Event that occurs whenever the default cancel button is pressed in the
 * dialog.
 *
 * @return TRUE to close the dialog. FALSE to prevent it from closing.
 */
bool PropertiesDialog::OnCancel() {
	return true;
}

/**
 * Event that occurs whenever the user changes the value of the Date Time
 * Picker.
 *
 * @param dtc Date Time Picker change object from the WM_NOTIFY message.
 *
 * @return TRUE if we handled the message. FALSE otherwise.
 */
INT_PTR PropertiesDialog::OnDateTimeChange(LPNMDATETIMECHANGE dtc) {
	if (dtc->dwFlags == GDT_VALID)
		m_st = dtc->st;
	return TRUE;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                Helpers                                    |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the content of an edit field in the dialog.
 *
 * @warning This method allocates memory dynamically.
 *
 * @param hwndEdit Window handle of the edit field.
 *
 * @return Contents of the edit field.
 */
LPTSTR PropertiesDialog::GetFieldText(HWND hwndEdit) const {
	// Get the length of the required string.
	int nLen = GetWindowTextLength(hwndEdit);
	if (nLen == 0)
		return NULL;
	nLen++;  // To include NUL terminator.

	// Allocate the memory required for the string.
	LPTSTR szText = (LPTSTR)malloc(nLen * sizeof(TCHAR));
	if (szText == NULL)
		return NULL;

	// Get the text from the control.
	GetWindowText(hwndEdit, szText, nLen);

	return szText;
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
INT_PTR CALLBACK PropertiesDialog::DlgProc(HWND hDlg, UINT wMsg, WPARAM wParam,
										   LPARAM lParam) {
	// Handle messages.
	switch (wMsg) {
		case WM_INITDIALOG:
			if (OnInit(hDlg))
				return TRUE;
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
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
				if (((LPNMHDR)lParam)->idFrom == IDC_DP_DTPTIMESTAMP)
					return OnDateTimeChange((LPNMDATETIMECHANGE)lParam);
				break;
			}
			break;
	}

	// Pass the message to the default message handler.
	return DefaultDlgProc(hDlg, wMsg, wParam, lParam);
}
