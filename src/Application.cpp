/**
 * Application.cpp
 * The Windows port of the BOLOTA project.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Application.h"

#include <windowsx.h>
#include <ShellAPI.h>
#ifdef DEBUG
	#include <crtdbg.h>
#endif // DEBUG

#include "AboutDialog.h"
#include "MainWindow.h"

// Common definitions.
#define MAX_LOADSTRING 100

// Global variables.
MainWindow *wndMain = NULL;
TCHAR szWindowClass[MAX_LOADSTRING];
TCHAR szAppTitle[MAX_LOADSTRING];

/**
 * Application's main entry point.
 *
 * @param hInstance     Program instance.
 * @param hPrevInstance Ignored: Leftover from Win16.
 * @param lpCmdLine     String with command line text.
 * @param nShowCmd      Initial state of the program's main window.
 *
 * @return wParam of the WM_QUIT message.
 */
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					  LPTSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	HACCEL hAccel;
	int rc;
	
	// Ensure we specify parameters not in use.
	UNREFERENCED_PARAMETER(hPrevInstance);

#ifdef DEBUG
	// Initialize memory leak detection.
	_CrtMemState snapBegin;
	_CrtMemState snapEnd;
	_CrtMemState snapDiff;
	_CrtMemCheckpoint(&snapBegin);
#endif // DEBUG

	// Load the application class and title.
	LoadString(hInstance, IDC_BOLOTA, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szAppTitle, MAX_LOADSTRING);

	// Initialize the application.
	rc = RegisterApplication(hInstance);
	if (rc == 0) {
		MsgBoxError(NULL, _T("Error Registering Class"),
			_T("An error occurred while trying to register the application's ")
			_T("window class."));
		return 0;
	}

	// Initialize this single instance.
	HWND hwndMain = InitializeInstance(hInstance, lpCmdLine, nCmdShow);
	if (hwndMain == 0) {
		MsgBoxError(NULL, _T("Error Initializing Instance"),
			_T("An error occurred while trying to initialize the ")
			_T("application's instance."));
		return 0x10;
	}

	// Load accelerators.
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOLOTA));

	// Application message loop.
	while (GetMessage(&msg, NULL, 0, 0)) {
		// Translate accelerators.
		if (!TranslateAccelerator(hwndMain, hAccel, &msg)) {
			// Translate message.
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Terminate instance.
	rc = TerminateInstance(hInstance, (int)msg.wParam);

#ifdef DEBUG
	// Detect memory leaks.
	_CrtMemCheckpoint(&snapEnd);
	if (_CrtMemDifference(&snapDiff, &snapBegin, &snapEnd)) {
		OutputDebugString(_T("MEMORY LEAKS DETECTED\r\n"));
		OutputDebugString(L"----------- _CrtMemDumpStatistics ---------\r\n");
		_CrtMemDumpStatistics(&snapDiff);
		OutputDebugString(L"----------- _CrtMemDumpAllObjectsSince ---------\r\n");
		_CrtMemDumpAllObjectsSince(&snapBegin);
		OutputDebugString(L"----------- _CrtDumpMemoryLeaks ---------\r\n");
		_CrtDumpMemoryLeaks();
	} else {
		OutputDebugString(_T("No memory leaks detected. Congratulations!\r\n"));
	}
#endif // DEBUG

	return rc;
}

/**
 * Initializes the application and registers the application class.
 *
 * @param hInstance Application instance.
 *
 * @return TRUE if the class was registered.
 */
ATOM RegisterApplication(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	// Setup the application's main window class.
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MainWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_BOLOTA);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
	wcex.lpszMenuName	= (LPCTSTR)IDC_BOLOTA;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	// Register the application's main window class.
	return RegisterClassEx(&wcex);
}

/**
 * Initializes the instance and creates the window.
 *
 * @param hInstance Program instance.
 * @param lpCmdLine String with command line text.
 * @param nShowCmd  Initial state of the program's main window.
 *
 * @return Window handler.
 */
HWND InitializeInstance(HINSTANCE hInstance, LPTSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;

	// Initialize main window object.
	int numArgs = 0;
	LPTSTR *lpArgs = CommandLineToArgvW(lpCmdLine, &numArgs);
	LPCTSTR szAddress = NULL;
	if (numArgs > 1)
		szAddress = lpArgs[1];
	wndMain = new MainWindow(hInstance, szAddress);
	LocalFree(lpArgs);
	lpArgs = NULL;
	szAddress = NULL;

	// Create the main window.
	hWnd = CreateWindow(szWindowClass,			// Window class.
						szAppTitle,				// Window title.
						WS_OVERLAPPEDWINDOW,	// Style flags.
						CW_USEDEFAULT,			// X position.
						CW_USEDEFAULT,			// Y position.
						600,					// Initial width,
						400,					// Initial height.
						NULL,					// Parent window.
						NULL,					// Menu class. (Always NULL)
						hInstance,				// Application instance.
						NULL);					// Pointer to create parameters.

	// Check if the window creation worked.
	if (!IsWindow(hWnd)) {
		MsgBoxError(NULL, _T("Error Initializing Instance"),
			_T("Window creation failed."));
		return NULL;
	}

#ifdef UNDERCE
	// Set the window task switching icon.
	HANDLE hIcon = LoadImage(hInst, MAKEINTRESOURCE(IDI_DESKTOP), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	// Set window taskbar icon.
	hIcon = LoadImage(hInst, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
#endif

	// Show and update the window.
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

/**
 * Terminates the application instance.
 *
 * @param hInstance Application instance.
 * @param nDefRC    Return code.
 *
 * @return Previous return code.
 */
int TerminateInstance(HINSTANCE hInstance, int nDefRC) {
	return nDefRC;
}

/**
 * Main window procedure.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Message parameter.
 * @param lParam Message parameter.
 *
 * @return 0 if everything worked.
 */
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam,
								LPARAM lParam) {
	switch (wMsg) {
		case WM_CREATE:
			return WndMainCreate(hWnd, wMsg, wParam, lParam);
		case WM_COMMAND:
			return WndMainCommand(hWnd, wMsg, wParam, lParam);
		case WM_CONTEXTMENU:
			return WndMainContextMenu(hWnd, wMsg, wParam, lParam);
		case WM_NOTIFY:
			return WndMainNotify(hWnd, wMsg, wParam, lParam);
		case WM_SIZE:
			return WndMainSize(hWnd, wMsg, wParam, lParam);
		case WM_CLOSE:
			return WndMainClose(hWnd, wMsg, wParam, lParam);
		case WM_DESTROY:
			return WndMainDestroy(hWnd, wMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

/**
 * Process the WM_CREATE message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Message parameter.
 * @param lParam Message parameter.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainCreate(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// Setup the window.
	wndMain->SetupControls(hWnd);
	return 0;
}

/**
 * Process the WM_COMMAND message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Message parameter.
 * @param lParam Message parameter.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainCommand(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	UINT_PTR wmId = LOWORD(wParam);
	UINT_PTR wmEvent = HIWORD(wParam);

	// Handle menu item commands.
	if ((wmId > IDM_BASE) && (wmId < IDC_BASE))
		return wndMain->OnMenuCommand(wmId, wmEvent);

	switch (wmId) {
		case IDM_ABOUT:
			AboutDialog(wndMain->hInst, hWnd).ShowModal();
			return 0;
		case IDM_EXIT:
			PostMessage(wndMain->hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
			return 0;
		default:
			return DefWindowProc(hWnd, wMsg, wParam, lParam);
	}
}

/**
 * Process the WM_CONTEXTMENU message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Handle of the window that was right-clicked. Can be a child
  *              window of the window receiving the message.
 * @param lParam LOWORD specifies the X position of the cursor in screen
 *               coordinates, HIWORD specifies the Y position.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainContextMenu(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// Break the parameters down.
	HWND hwndParam = (HWND)wParam;
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam); 
	
	// Let our main window wrapper deal with the message.
	if (wndMain->OnContextMenu(hwndParam, xPos, yPos))
		return 0;

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

/**
 * Process the WM_NOTIFY message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Identifier of the common control sending the message. Not
 *               always unique. hwndFrom or idFrom of the NMHDR should be used.
 * @param lParam Pointer to an NMHDR structure containing the notification code
 *               and additional information.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainNotify(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	LPNMHDR nmh = (LPNMHDR)lParam;
	switch (nmh->code) {
	default:
		return DefWindowProc(hWnd, wMsg, wParam, lParam);
	}
}

/**
 * Process the WM_SIZE message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam Specifies the type of resizing requested.
 * @param lParam New width (LOWORD) and height (HIWORD) of the client area.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainSize(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// We have been minimized, so nothing should be done.
	if (wParam == SIZE_MINIMIZED)
		return DefWindowProc(hWnd, wMsg, wParam, lParam);

	// Resize child windows.
	wndMain->ResizeWindows(hWnd);

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

/**
 * Process the WM_CLOSE message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam This parameter is not used.
 * @param lParam This parameter is not used.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainClose(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// Check if we can close.
	if (!wndMain->OnClose())
		return 0;

	// Send main window destruction message.
	delete wndMain;

	// Call any destructors that might be needed.
	// TODO: Call destructors.

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

/**
 * Process the WM_DESTROY message for the window.
 *
 * @param hWnd   Window handler.
 * @param wMsg   Message type.
 * @param wParam This parameter is not used.
 * @param lParam This parameter is not used.
 *
 * @return 0 if everything worked.
 */
LRESULT WndMainDestroy(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// Post quit message and return.
	PostQuitMessage(0);
	return 0;
}
