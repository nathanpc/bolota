/**
 * Application.cpp
 * The Windows port of the BOLOTA project.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Application.h"

#include <windowsx.h>
#include <ShellAPI.h>
#if defined(DEBUG) && !defined(UNDER_CE)
	#include <crtdbg.h>
#endif // defined(DEBUG) && !defined(UNDER_CE)

#include "../shims/cvtutf/Unicode.h"
#include "MainWindow.h"
#include "Utilities/Settings/ConfigManager.h"

using namespace Settings;

// Global variables.
static ConfigManager* configManager = NULL;
static MainWindow *wndMain = NULL;
static TCHAR szWindowClass[20];
static TCHAR szAppTitle[20];

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
	HWND hwndMain;
	int rc;
	
	// Ensure we specify parameters not in use.
	UNREFERENCED_PARAMETER(hPrevInstance);

#if defined(DEBUG) && !defined(UNDER_CE)
	// Initialize memory leak detection.
	_CrtMemState snapBegin;
	_CrtMemState snapEnd;
	_CrtMemState snapDiff;
	_CrtMemCheckpoint(&snapBegin);
#endif // defined(DEBUG) && !defined(UNDER_CE)

	// Check if we can perform proper Unicode conversions.
	if (!Unicode::AssumptionsCheck()) {
		MsgBoxError(NULL, _T("Unicode Conformance Issue"),
			_T("Either the size of wchar_t is not 2 bytes long as required ")
			_T("for UTF-16, or char is not 1 byte long, as per UTF-8. All ")
			_T("Unicode conversions would fail."));
		return 1;
	}

	// Initialize application's singletons.
	Bolota::ErrorStack* errorStack = Bolota::ErrorStack::Instance();
	Bolota::FieldTypeList* listFieldTypes = Bolota::FieldTypeList::Instance();
	configManager = Settings::ConfigManager::Instance();

	// Load the application class and title.
	LoadString(hInstance, IDS_APP_CLASS, szWindowClass, 20);
	LoadString(hInstance, IDS_APP_TITLE, szAppTitle, 20);

	// Initialize the application.
	rc = RegisterApplication(hInstance);
	if (rc == 0) {
		MsgBoxError(NULL, _T("Error Registering Class"),
			_T("An error occurred while trying to register the application's ")
			_T("window class."));
		rc = 1;
		goto terminate;
	}

	// Initialize this single instance.
	hwndMain = InitializeInstance(hInstance, lpCmdLine, nCmdShow);
	if (hwndMain == 0) {
		MsgBoxError(NULL, _T("Error Initializing Instance"),
			_T("An error occurred while trying to initialize the ")
			_T("application's instance."));
		rc = 0x10;
		goto terminate;
	}

	// Load accelerators.
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

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

terminate:
	// Clean up our singletons.
	delete errorStack;
	errorStack = NULL;
	delete listFieldTypes;
	listFieldTypes = NULL;
	delete configManager;
	configManager = NULL;

#if defined(DEBUG) && !defined(UNDER_CE)
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
#endif // defined(DEBUG) && !defined(UNDER_CE)

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
#ifdef SHELL_AYGSHELL
	// Only allow one instance of the application under Pocket PC.
	HWND hWnd = FindWindow(szWindowClass, NULL);
	if (hWnd) {
		SetForegroundWindow((HWND)(((DWORD)hWnd) | 0x01));
		return FALSE;
	}
#endif // SHELL_AYGSHELL

#ifndef UNDER_CE
	// Setup the application's main window class.
	WNDCLASSEX wcex;
	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MainWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOLOTA));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= GetSysColorBrush(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDM_MAIN);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Register the application's main window class.
	return RegisterClassEx(&wcex);
#else
	// Setup the application's main window class.
	WNDCLASS wc;
	wc.style         = CS_VREDRAW | CS_HREDRAW;   // Window style.
	wc.lpfnWndProc   = MainWindowProc;            // Main window procedure.
	wc.cbClsExtra    = 0;                         // Extra class data.
	wc.cbWndExtra    = 0;                         // Extra window data.
	wc.hInstance     = hInstance;                 // Owner handle.
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOLOTA));
	wc.hCursor       = NULL;                      // Default cursor. (Always NULL)
	wc.hbrBackground = GetSysColorBrush(COLOR_STATIC);
	wc.lpszMenuName  = NULL;                      // Menu name. (Always NULL)
	wc.lpszClassName = szWindowClass;             // Window class name.

	// Register the application's main window class.
	if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error",
			MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

	return TRUE;
#endif // !UNDER_CE
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

#ifdef SHELL_AYGSHELL
	// Initialize PocketPC controls.
	SHInitExtraControls();
#endif

	// Initialize main window object.
#ifndef UNDER_CE
	int numArgs = 0;
	LPTSTR *lpArgs = CommandLineToArgvW(lpCmdLine, &numArgs);
	LPCTSTR szAddress = NULL;
	if (numArgs > 1)
		szAddress = lpArgs[1];
	wndMain = new MainWindow(hInstance, szAddress);
	LocalFree(lpArgs);
	lpArgs = NULL;
	szAddress = NULL;
#else
	wndMain = new MainWindow(hInstance, NULL);
#endif // !UNDER_CE

	// Create the main window.
#ifndef UNDER_CE
	// Get window size from settings.
	DWordSetting* dwsWidth = configManager->Get<DWordSetting>(
		ConfigManager::WindowWidth);
	DWordSetting* dwsHeight = configManager->Get<DWordSetting>(
		ConfigManager::WindowHeight);

	hWnd = CreateWindow(szWindowClass,			// Window class.
						szAppTitle,				// Window title.
						WS_OVERLAPPEDWINDOW,	// Style flags.
						CW_USEDEFAULT,			// X position.
						CW_USEDEFAULT,			// Y position.
						dwsWidth->Value(),		// Initial width,
						dwsHeight->Value(),		// Initial height.
						NULL,					// Parent window.
						NULL,					// Menu class. (Always NULL)
						hInstance,				// Application instance.
						NULL);					// Pointer to create parameters.
#else
	hWnd = CreateWindow(szWindowClass, szAppTitle, WS_VISIBLE, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance,
		NULL);
#endif // !UNDER_CE

	// Check if the window creation worked.
	if (!IsWindow(hWnd)) {
		MsgBoxError(NULL, _T("Error Initializing Instance"),
			_T("Window creation failed."));
		return NULL;
	}

#ifdef SHELL_AYGSHELL
	// Take menu bar height into account.
	RECT rcMB;
	GetWindowRect(hWnd, &rcMB);
	rcMB.bottom -= 26;
	MoveWindow(hWnd, rcMB.left, rcMB.top, rcMB.right, rcMB.bottom, FALSE);
#endif // SHELL_AYGSHELL

#ifdef UNDER_CE
	// Set the window task switching icon.
	HANDLE hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_BOLOTA),
		IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
		0);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	// Set window taskbar icon.
	hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
#endif // UNDER_CE

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
#ifndef UNDER_CE
		case WM_CONTEXTMENU:
			return WndMainContextMenu(hWnd, wMsg, wParam, lParam);
#endif // !UNDER_CE
		case WM_NOTIFY:
			return WndMainNotify(hWnd, wMsg, wParam, lParam);
		case WM_SIZE:
			return WndMainSize(hWnd, wMsg, wParam, lParam);
#ifdef SHELL_AYGSHELL
		case WM_ACTIVATE:
			SHHandleWMActivate(hWnd, wParam, lParam,
				&wndMain->CommandBar()->SHActivateInfo(), 0);
			break;
		case WM_SETTINGCHANGE:
			SHHandleWMSettingChange(hWnd, wParam, lParam,
				&wndMain->CommandBar()->SHActivateInfo());
			break;
#endif // SHELL_AYGSHELL
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

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
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
#ifndef UNDER_CE
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
#else
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);
#endif // !UNDER_CE
	
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
