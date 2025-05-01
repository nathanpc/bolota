/**
 * CECommandBar.cpp
 * Handles the CommandBar used in some Windows CE devices.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "CECommandBar.h"

using namespace WinCE;

// Number of bitmaps in the toolbar's ImageList.
#define TBL_BMPS_LEN 14

/**
 * Toolbar buttons in the CommandBar.
 */
const TBBUTTON tbButtons[] = {
// BmpIdx  Command                 State            Style       UserData String
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 0,   IDM_FILE_NEW,           TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 1,   IDM_FILE_OPEN,          TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 2,   IDM_FILE_SAVE,          TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 3,   IDM_FILE_RELOAD,        TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 4,   IDM_FILE_PROPERTIES,    TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 5,   IDM_FIELD_EDIT,         TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 6,   IDM_FIELD_DELETE,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 7,   IDM_FIELD_PREPEND,      TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 8,   IDM_FIELD_APPEND,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 9,   IDM_FIELD_CREATECHILD,  TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 10,  IDM_FIELD_MOVEUP,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 11,  IDM_FIELD_MOVEDOWN,     TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,   0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ 12,  IDM_FIELD_INDENT,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 13,  IDM_FIELD_DEINDENT,     TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 } 
};

/**
 * Creates and populates the Field icon ImageList.
 *
 * @param hInst      Application's instance handle.
 * @param hwndParent Parent window that in which the bar will attach to.
 */
CommandBar::CommandBar(HINSTANCE hInst, HWND hwndParent) {
	// Store its parent's state.
	this->hInst = hInst;
	this->hwndParent = hwndParent;

#ifndef SHELL_AYGSHELL
	// Create CommandBar and add bitmaps to its ImageList.
	this->hWnd = CommandBar_Create(hInst, hwndParent, IDC_CMDBAR);
	CommandBar_AddBitmap(this->hWnd, this->hInst, IDB_TBCLASSIC, TBL_BMPS_LEN,
		16, 16);

	// Insert menu bar, toolbar buttons, and the exit button.
	CommandBar_InsertMenubar(this->hWnd, hInst, IDM_MAIN, 0);
	CommandBar_AddButtons(this->hWnd, sizeof(tbButtons) / sizeof(TBBUTTON),
		tbButtons);
	CommandBar_AddAdornments(this->hWnd, 0, 0);
#else
	SHMENUBARINFO mbi = {0};

	// Setup the menu bar.
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwndParent;         // Parent window.
	mbi.nToolBarId = IDM_MAIN;           // ID of the toolbar resource.
	mbi.hInstRes   = hInst;              // Instance handle of our application.
	mbi.nBmpId     = 0;                  // Bitmap resource ID.
	mbi.cBmpImages = 0;                  // Number of images in the bitmap.
	mbi.dwFlags    = SHCMBF_HMENU;       // nToolBarId is a menu resource.
	
	// Create the menu bar.
	if (!SHCreateMenuBar(&mbi)) {
		MsgBoxError(hWnd, _T("UI Error"), _T("Couldn't create the menu bar."));
		MsgBoxLastError(hWnd);
		DestroyWindow(hWnd);
	}

	// Save the menu bar handle.
	this->hWnd = mbi.hwndMB;

#ifndef WIN32_PLATFORM_WFSP
	// Initialize the shell to activate the info structure.
	memset(&m_sai, 0, sizeof(SHACTIVATEINFO));
	m_sai.cbSize = sizeof(SHACTIVATEINFO);
#endif // !WIN32_PLATFORM_WFSP
#endif // !SHELL_AYGSHELL
}

/**
 * Destroys the window and cleans things up.
 */
CommandBar::~CommandBar() {
	// Dispose of ourselves.
	if (this->hWnd)
		CommandBar_Destroy(this->hWnd);
	this->hWnd = NULL;
}

/**
 * Gets the bar's height. Useful for offsetting controls so they clear us.
 *
 * @return Bar's height.
 */
LONG CommandBar::Height() const {
#ifndef SHELL_AYGSHELL
	return CommandBar_Height(this->hWnd);
#else
	RECT rcMenuBar;
	GetWindowRect(this->hWnd, &rcMenuBar);
	return rcMenuBar.bottom - rcMenuBar.top;
#endif // SHELL_AYGSHELL
}

#ifdef SHELL_AYGSHELL
/**
 * Gets the visible desktop area where we can draw our window when the SIP is
 * open and visible.
 *
 * @return Visible desktop area in client coordinates.
 */
RECT CommandBar::SIPVisibleDesktop() const {
	SIPINFO si = {0};

	// Query the SIP state.
	si.cbSize = sizeof(si);
	SHSipInfo(SPI_GETSIPINFO, 0, (PVOID)&si, 0);

	return si.rcVisibleDesktop;
}
#endif // SHELL_AYGSHELL

/**
 * Gets the handle of the associated CommandBar control.
 *
 * @return CommandBar control window handle.
 */
HWND CommandBar::Handle() const {
	return this->hWnd;
}

/**
 * Gets the handle of the menu in the CommandBar control.
 *
 * @return CommandBar menu handle.
 */
HMENU CommandBar::Menu() const {
#ifdef SHELL_AYGSHELL
	return (HMENU)SendMessage(this->hWnd, SHCMBM_GETMENU, 0, 0);
#else
	return CommandBar_GetMenu(this->hWnd, 0);
#endif // SHELL_AYGSHELL
}

#ifdef SHELL_AYGSHELL
/**
 * Gets the shell activate information about the Pocket PC user interface.
 *
 * @return Our internal SHACTIVATEINFO instance.
 */
SHACTIVATEINFO CommandBar::SHActivateInfo() const {
	return this->m_sai;
}
#endif // SHELL_AYGSHELL
