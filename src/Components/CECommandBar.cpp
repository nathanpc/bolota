/**
 * CECommandBar.cpp
 * Handles the CommandBar used in some Windows CE devices.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "CECommandBar.h"

using namespace WinCE;

// Number of bitmaps in the standard and view ImageLists.
#define STD_BMPS_LEN  (STD_PRINT + 1)
#define BOFS          (STD_BMPS_LEN)

/**
 * Toolbar buttons in the CommandBar.
 */
const TBBUTTON tbButtons[] = {
//    BitmapIndex     Command                 State            Style       UserData String
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ STD_FILENEW,    IDM_FILE_NEW,           TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ STD_FILEOPEN,   IDM_FILE_OPEN,          TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ STD_FILESAVE,   IDM_FILE_SAVE,          TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ BOFS + 0,       IDM_FILE_RELOAD,        TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ STD_PROPERTIES, IDM_FILE_PROPERTIES,    TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ BOFS + 1,       IDM_FIELD_EDIT,         TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ BOFS + 2,       IDM_FIELD_DELETE,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ BOFS + 3,       IDM_FIELD_PREPEND,      TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ BOFS + 4,       IDM_FIELD_APPEND,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ BOFS + 5,       IDM_FIELD_CREATECHILD,  TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ BOFS + 6,       IDM_FIELD_MOVEUP,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ BOFS + 7,       IDM_FIELD_MOVEDOWN,     TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ 0,              0,                      0,               TBSTYLE_SEP,       0,   0 },
	{ BOFS + 8,       IDM_FIELD_INDENT,       TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 },
	{ BOFS + 9,       IDM_FIELD_DEINDENT,     TBSTATE_ENABLED, TBSTYLE_BUTTON,    0,   0 } 
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
	// Create CommandBar.
	this->hWnd = CommandBar_Create(hInst, hwndParent, IDC_CMDBAR);

	// Add the Standard and View bitmaps to the toolbar just in case.
	CommandBar_AddBitmap(this->hWnd, HINST_COMMCTRL, IDB_STD_SMALL_COLOR,
		STD_BMPS_LEN, 16, 16);
	CommandBar_AddBitmap(this->hWnd, this->hInst, IDB_TBCLASSIC, 10, 16, 16);

	// Insert menu bar, toolbar buttons, and the exit button.
	CommandBar_InsertMenubar(this->hWnd, hInst, IDM_MAIN, 0);
	CommandBar_AddButtons(this->hWnd, sizeof(tbButtons) / sizeof(TBBUTTON),
		tbButtons);
	CommandBar_AddAdornments(this->hWnd, 0, 0);
#else
	SHMENUBARINFO mbi = {0};

	// Initialize the shell to activate the info structure.
	memset(&m_sai, 0, sizeof(SHACTIVATEINFO));
	m_sai.cbSize = sizeof(SHACTIVATEINFO);

	// Setup the menu bar.
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hWnd;               // Parent window.
	mbi.nToolBarId = IDM_MAIN;           // ID of the toolbar resource.
	mbi.hInstRes   = hInst;              // Instance handle of our application.
	mbi.nBmpId     = 0;                  // Bitmap resource ID.
	mbi.cBmpImages = 0;                  // Number of images in the bitmap.
	
	// Create the menu bar.
	if (!SHCreateMenuBar(&mbi)) {
		MsgBoxError(hWnd, _T("UI Error"), _T("Couldn't create the menu bar."));
		DestroyWindow(hWnd);
	}

	// Save the menu bar handle.
	this->hWnd = mbi.hwndMB;
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
	SIPINFO si = {0};
	int cx, cy;

	// Query the SIP state and size our window appropriately.
	si.cbSize = sizeof(si);
	SHSipInfo(SPI_GETSIPINFO, 0, (PVOID)&si, 0);
	cx = si.rcVisibleDesktop.right - si.rcVisibleDesktop.left;
	cy = si.rcVisibleDesktop.bottom - si.rcVisibleDesktop.top;

	// Correct the window height based on the menu bar height.
	if (!(si.fdwFlags & SIPF_ON) || ((si.fdwFlags & SIPF_ON) &&
			(si.fdwFlags & SIPF_DOCKED))) {
		RECT rcMenuBar;
		GetWindowRect(this->hWnd, &rcMenuBar);

		cy -= (rcMenuBar.bottom - rcMenuBar.top);
	}

	return cy;
#endif // SHELL_AYGSHELL
}

/**
 * Gets the handle of the associated CommandBar control.
 *
 * @return CommandBar control window handle.
 */
HWND CommandBar::Handle() const {
	return this->hWnd;
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
