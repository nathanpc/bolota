/**
 * Toolbar.cpp
 * Handles the toolbar of the application.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Toolbar.h"

// Some local constants.
#define TBR_BIT_DEPTH     ILC_COLORDDB
#define TBR_ICON_SIZE     GetSystemMetrics(SM_CXSMICON)
#define TBR_IMAGELIST_LEN 5

/**
 * Creates and prepares the application's toolbar.
 *
 * @param hInst      Application's instance handle.
 * @param hwndParent Parent window that in which the bar will attach to.
 */
Toolbar::Toolbar(HINSTANCE hInst, HWND hwndParent) {
	// Store its parent's state.
	this->hInst = hInst;
	this->hwndParent = hwndParent;

	// Clear things up.
	m_imlToolbar = NULL;
	m_hwndRebar = NULL;
	m_hwndTbrFile = NULL;
	m_hwndTbrTopics = NULL;
	
	// Create and populate all the controls associated with the toolbar.
	CreateImageList();
	CreateRebar();
}

/**
 * Destroys the toolbar and cleans things up.
 */
Toolbar::~Toolbar() {
	// Dispose of the Rebar.
	if (m_hwndRebar)
		DestroyWindow(m_hwndRebar);
	m_hwndRebar = NULL;

	// Dispose of the ImageList.
	if (m_imlToolbar)
		delete m_imlToolbar;
	m_imlToolbar = NULL;

	// Dispose of the toolbars.
	if (m_hwndTbrFile)
		DestroyWindow(m_hwndTbrFile);
	m_hwndTbrFile = NULL;
	if (m_hwndTbrTopics)
		DestroyWindow(m_hwndTbrTopics);
	m_hwndTbrTopics = NULL;
}

/**
 * Creates and populates the toolbar's ImageList.
 */
void Toolbar::CreateImageList() {
	// Create the ImageList.
	m_imlToolbar = new ImageList(this->hInst, TBR_ICON_SIZE, TBR_BIT_DEPTH,
		TBR_IMAGELIST_LEN);
}

/**
 * Creates and populates the file toolbar with controls.
 *
 * @param lpSize Optional. Size of the Toolbar with all of its controls.
 *
 * @return Window handle for the Toolbar control.
 */
HWND Toolbar::CreateFileToolbar(LPSIZE lpSize) {
	// Create the Toolbar.
	const DWORD buttonStyles = BTNS_AUTOSIZE;
	m_hwndTbrFile = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_LIST |
		TBSTYLE_TRANSPARENT | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE, 0, 0, 0, 0, this->hwndParent,
		(HMENU)IDC_TBRFILE, this->hInst, NULL);
	if (m_hwndTbrFile == NULL) {
		MsgBoxError(NULL, _T("Error creating file toolbar"),
			_T("An error occurred while trying to CreateWindowEx for the ")
			_T("file toolbar."));
		return NULL;
	}
	SendMessage(m_hwndTbrFile, TB_SETEXTENDEDSTYLE, (WPARAM)NULL,
		(LPARAM)TBSTYLE_EX_MIXEDBUTTONS);

	// Associate the ImageList to the toolbar.
	SendMessage(m_hwndTbrFile, TB_SETIMAGELIST, (WPARAM)0,
		(LPARAM)m_imlToolbar->Handle());

	// Set up Toolbar buttons.
	const int numButtons = 5;
	int ibl = SendMessage(m_hwndTbrFile, TB_ADDSTRING, (WPARAM)hInst,
		(LPARAM)IDS_TBFILELABELS);
	TBBUTTON tbButtons[numButtons] = {
		{ 0, IDM_FILE_NEW, TBSTATE_ENABLED, buttonStyles, {0}, NULL, ibl++ },
		{ 1, IDM_FILE_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, NULL, ibl++ },
		{ 2, IDM_FILE_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, NULL, ibl++ },
		{ 3, IDM_FILE_RELOAD, TBSTATE_ENABLED, buttonStyles, {0}, NULL, ibl++ },
		{ 4, IDM_FILE_PROPERTIES, TBSTATE_ENABLED, buttonStyles, {0}, NULL, ibl++ }
	};

	// Add buttons.
	SendMessage(m_hwndTbrFile, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON),
		(LPARAM)0);
	SendMessage(m_hwndTbrFile, TB_ADDBUTTONS, (WPARAM)numButtons,
		(LPARAM)&tbButtons);

	// Resize the Toolbar, and then show it.
	SendMessage(m_hwndTbrFile, TB_AUTOSIZE, 0, 0);
	ShowWindow(m_hwndTbrFile, TRUE);

	// Get the final size of the Toolbar if requested.
	if (lpSize != NULL)
		SendMessage(m_hwndTbrFile, TB_GETMAXSIZE, 0, (LPARAM)lpSize);

	return m_hwndTbrFile;
}

/**
 * Creates and populates a Rebar with controls. 
 *
 * @return Window handle for the Rebar control.
 */
HWND Toolbar::CreateRebar() {
	// Create Rebar control.
	m_hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		RBS_VARHEIGHT | CCS_NODIVIDER | RBS_BANDBORDERS | RBS_AUTOSIZE,
		0, 0, 0, 0, this->hwndParent, (HMENU)IDC_CMDBAR, hInst, NULL);
	if (m_hwndRebar == NULL) {
		MsgBoxError(NULL, _T("Error creating Rebar"),
			_T("An error occurred while trying to CreateWindowEx the Rebar."));
		return NULL;
	}

	// Create file toolbar, get its size, and append it to the Rebar.
	SIZE sizeToolbar;
	if (CreateFileToolbar(&sizeToolbar) == NULL)
		return NULL;
	InsertToolbar(m_hwndTbrFile, sizeToolbar);

	return m_hwndRebar;
}

/**
 * Inserts a band to the Rebar with a Toolbar control.
 * 
 * @param hwndToolbar Toolbar control to be inserted as a band.
 * @param size        Size of the toolbar.
 */
void Toolbar::InsertToolbar(HWND hwndToolbar, SIZE size) {
	// Initialize the band information.
	REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
	rbBand.fMask = RBBIM_STYLE | RBBIM_TEXT | RBBIM_CHILD | RBBIM_CHILDSIZE |
		RBBIM_SIZE;
	rbBand.fStyle = RBBS_CHILDEDGE;
	rbBand.lpText = (LPTSTR)_T("");
	rbBand.hwndChild = hwndToolbar;
	rbBand.cyChild = size.cy;
	rbBand.cxMinChild = size.cx;
	rbBand.cyMinChild = size.cy;
	rbBand.cx = 0;

	// Add the band to the Rebar control.
	SendMessage(m_hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
}

/**
 * Resizes the application's toolbar.
 *
 * @return Toolbar's client rectangle.
 */
RECT Toolbar::Resize(RECT rcParent) const {
	// Resize the Rebar.
	SetWindowPos(m_hwndRebar, NULL, 0, 0, rcParent.right - rcParent.left,
		20, SWP_NOZORDER | SWP_NOMOVE);

	// Get our client rectangle to help resize other controls.
	RECT rcRebar;
	GetClientRect(m_hwndRebar, &rcRebar);

	return rcRebar;
}

/**
 * Gets the handle of the associated toolbar.
 *
 * @return Rebar control window handle.
 */
HWND Toolbar::Handle() const {
	return this->m_hwndRebar;
}
