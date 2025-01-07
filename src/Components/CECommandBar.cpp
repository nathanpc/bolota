/**
 * CECommandBar.cpp
 * Handles the CommandBar used in some Windows CE devices.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "CECommandBar.h"

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
		STD_PRINT + 1, 16, 16);
	CommandBar_AddBitmap(this->hWnd, HINST_COMMCTRL, IDB_VIEW_SMALL_COLOR,
		VIEW_NEWFOLDER + 1, 16, 16);

	// Insert menu bar, toolbar buttons, and the exit button.
	CommandBar_InsertMenubar(this->hWnd, hInst, IDM_MAIN, 0);
	//CommandBar_AddButtons(this->hWnd, sizeof(tbButtons) / sizeof(TBBUTTON),
	//	tbButtons);
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
