/**
 * MainWindow.cpp
 * Main window of the application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "MainWindow.h"

#include <commctrl.h>

using namespace Bolota;

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
	if (BolotaHasError) {
		MsgBoxBolotaError(hWnd, _T("Failed to initialize document viewer"));
		return FALSE;
	}
	m_wndBolota->OpenExampleDocument();
	if (BolotaHasError) {
		MsgBoxBolotaError(hWnd, _T("Failed to open example document"));
		return FALSE;
	}

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
 * Handles the WM_COMMAND messages from menu items and accelerators.
 *
 * @param wmId    ID of the control that sent the message.
 * @param wmEvent Extra event information.
 *
 * @return 0 if everything worked.
 */
LRESULT MainWindow::OnMenuCommand(UINT_PTR wmId, UINT_PTR wmEvent) {
	LRESULT lr = 0;

	switch (wmId) {
	case IDM_FILE_NEW:
		m_wndBolota->NewDocument();
		break;
	case IDM_FILE_SAVE:
		m_wndBolota->Save(false);
		break;
	case IDM_FILE_SAVEAS:
		m_wndBolota->Save(true);
		break;
	case IDM_FILE_OPEN:
		m_wndBolota->OpenFile();
		break;
	case IDM_FILE_RELOAD:
		lr = m_wndBolota->ReloadView();
		break;
	case IDM_FILE_PROPERTIES:
		lr = m_wndBolota->EditProperties();
		break;
	case IDM_FIELD_EDIT:
		lr = m_wndBolota->OpenFieldManager(
			FieldManagerDialog::DialogType::EditField);
		break;
	case IDM_FIELD_DELETE:
		lr = m_wndBolota->AskDeleteField();
		break;
	case IDM_FIELD_APPEND:
		lr = m_wndBolota->OpenFieldManager(
			FieldManagerDialog::DialogType::AppendField);
		break;
	case IDM_FIELD_PREPEND:
		lr = m_wndBolota->OpenFieldManager(
			FieldManagerDialog::DialogType::PrependField);
		break;
	case IDM_FIELD_CREATECHILD:
		lr = m_wndBolota->OpenFieldManager(
			FieldManagerDialog::DialogType::NewChildField);
		break;
	case IDM_FIELD_MOVEUP:
		lr = m_wndBolota->MoveField(true);
		break;
	case IDM_FIELD_MOVEDOWN:
		lr = m_wndBolota->MoveField(false);
		break;
	case IDM_FIELD_INDENT:
		lr = m_wndBolota->IndentField();
		break;
	case IDM_FIELD_DEINDENT:
		lr = m_wndBolota->DeindentField();
		break;
	default:
		MsgBoxInfo(this->hWnd, _T("Unknown Command ID"),
			_T("WM_COMMAND for MainWindow with unknown ID"));
	}

	// Handle errors.
	if (BolotaHasError)
		MsgBoxBolotaError(this->hWnd, _T("Operation failed"));

	return lr;
}

/**
 * Handles WM_CONTEXTMENU messages such as right-clicking or the keyboard menu
 * key being pressed.
 *
 * @param hWnd Handle of the window that was right-clicked.
 * @param xPos X position of the right-click in screen coordinates.
 * @param yPos Y position of the right-click in screen coordinates.
 *
 * @return TRUE if we handled the event. FALSE to let the default procedure take
 *         care of things for us.
 */
bool MainWindow::OnContextMenu(HWND hWnd, int xPos, int yPos) {
	// Check if the document viewer got the right-click.
	if (hWnd == m_wndBolota->WindowHandle())
		return m_wndBolota->ShowContextMenu(xPos, yPos);

	return false;
}

/**
 * Handles the WM_CLOSE message, allowing us to prevent a close operation in the
 * case of unsaved changes.
 *
 * @return TRUE if we should allow the window to close. FALSE otherwise.
 */
bool MainWindow::OnClose() const {
	return m_wndBolota->Close();
}
