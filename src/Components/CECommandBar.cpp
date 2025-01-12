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
	return CommandBar_Height(this->hWnd);
}

/**
 * Gets the handle of the associated CommandBar control.
 *
 * @return CommandBar control window handle.
 */
HWND CommandBar::Handle() const {
	return this->hWnd;
}
