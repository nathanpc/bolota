/**
 * MainWindow.cpp
 * Main window of the application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "MainWindow.h"

#include <commctrl.h>

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Constructs the main window object.
 *
 * @param hInstance Application instance.
 * @param szURI     Optional. Initial URI to load on open.
 */
MainWindow::MainWindow(HINSTANCE hInstance, LPCTSTR szURI) {
	// Initialize important stuff.
	this->hInst = hInstance;

	// Initialize default values.
	this->hWnd = NULL;
	m_wndBolota = NULL;
}

/**
 * Cleans up everything that was allocated by the main window.
 */
MainWindow::~MainWindow() {
	// Destroy the document viewer.
	delete m_wndBolota;
	m_wndBolota = NULL;

	// Destroy the main window.
	DestroyWindow(this->hWnd);
	this->hWnd = NULL;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             Controls Setup                                |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Sets up the layout of the window's controls.
 *
 * @param hWnd Main window handle.
 *
 * @return TRUE if the operation was successful.
 */
BOOL MainWindow::SetupControls(HWND hWnd) {
	// Ensure that the common controls DLL is loaded and initialized. 
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&icex);

	// Ensure we have a copy of the window handle.
	this->hWnd = hWnd;

	// Grab the dimensions of the client area.
	RECT rcClient;
	GetClientRect(this->hWnd, &rcClient);

	// Setup the document viewer.
	m_wndBolota = new BolotaView(this->hInst, this->hWnd, rcClient);
	m_wndBolota->OpenExampleDocument();

	return TRUE;
}

/**
 * Resizes all controls based on the parent's size.
 *
 * @param hwndParent Parent window of all controls.
 *
 * @return TRUE if the function succeeds.
 */
BOOL MainWindow::ResizeWindows(HWND hwndParent) {
	// Get the client area of the parent window.
	RECT rcParent;
	GetClientRect(hwndParent, &rcParent);

	// Resize document viewer.
	m_wndBolota->Resize(rcParent);

	return TRUE;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             Event Handlers                                |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Opens the appropriate field manager dialog window for the desired action.
 *
 * @param type Type of action to be performed in the manager dialog.
 *
 * @return 0 if everything worked.
 */
LRESULT MainWindow::OpenFieldManager(FieldManagerDialog::DialogType type) {
	// Get the currently selected field in the Tree-View.
	HTREEITEM hti;
	Bolota::Field *field = m_wndBolota->GetSelectedField(&hti);
	if (field == NULL) {
		MsgBoxError(this->hWnd, _T("No field selected"),
			_T("In order to perform this operation a field must be selected."));
		return 1;
	}

	// Setup and open the manager dialog.
	FieldManagerDialog dlgManager(this->hInst, this->hWnd, type, field);
	INT_PTR iRet = dlgManager.ShowModal();

	// Update the document viewer.
	switch (type) {
	case FieldManagerDialog::DialogType::EditField:
		m_wndBolota->RefreshField(hti, field);
		break;
	default:
		MsgBoxError(this->hWnd, _T("Unknown operation type"), _T("Unable to ")
			_T("perform post-dialog operation on unknown operation type."));
	}

	return 0;
}

/**
 * Handles the WM_COMMAND messages from menu items and accelerators.
 *
 * @param wmId    ID of the control that sent the message.
 * @param wmEvent Extra event information.
 *
 * @return 0 if everything worked.
 */
LRESULT MainWindow::OnMenuCommand(UINT_PTR wmId, UINT_PTR wmEvent) {
	switch (wmId) {
	case IDM_FIELD_EDIT:
		return OpenFieldManager(FieldManagerDialog::DialogType::EditField);
	case IDM_FIELD_APPEND:
		return OpenFieldManager(FieldManagerDialog::DialogType::AppendField);
	case IDM_FIELD_PREPEND:
		return OpenFieldManager(FieldManagerDialog::DialogType::PrependField);
	case IDM_FIELD_CREATECHILD:
		return OpenFieldManager(FieldManagerDialog::DialogType::NewChildField);
	default:
		MsgBoxInfo(this->hWnd, _T("Unknown Command ID"),
			_T("WM_COMMAND for MainWindow with unknown ID"));
		return 0;
	}
}
