/**
 * Toolbar.h
 * Handles the toolbar of the application.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _COMPONENTS_TOOLBAR_H
#define _COMPONENTS_TOOLBAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../stdafx.h"
#include "../Utilities/ImageList.h"

/**
 * Handles the toolbar of the application.
 */
class Toolbar {
protected:
	HINSTANCE hInst;
	HWND hwndParent;

	ImageList* m_imlToolbar;
	HWND m_hwndRebar;
	HWND m_hwndTbrFile;
	HWND m_hwndTbrTopics;

	// Initializers
	void CreateImageList();
	HWND CreateRebar();
	HWND CreateFileToolbar(LPSIZE lpSize);
	HWND CreateTopicsToolbar(LPSIZE lpSize);

	// Helpers
	void InsertToolbar(HWND hwndToolbar, SIZE size);

public:
	// Constructors and destructors.
	Toolbar(HINSTANCE hInst, HWND hwndParent);
	virtual ~Toolbar();

	// Helpers
	RECT Resize(RECT rcParent) const;
	HWND Handle() const;
};

#endif // _COMPONENTS_TOOLBAR_H
