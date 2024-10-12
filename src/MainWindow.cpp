/**
 * MainWindow.cpp
 * Main window of the application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "MainWindow.h"

#include <commctrl.h>

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
