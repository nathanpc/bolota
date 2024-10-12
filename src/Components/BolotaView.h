/**
 * BolotaView.h
 * A TreeView component that displays and handles a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_BOLOTAVIEW_H
#define _BOLOTA_BOLOTAVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <commctrl.h>

#include "../Bolota/Document.h"

/**
* A TreeView component that displays and handles a Bolota document.
*/
class BolotaView {
private:
	// Data sources.
	Bolota::Document *m_doc;

	// Window handles.
	HWND m_hWnd;

public:
	// Constructors and destructors.
	BolotaView(HINSTANCE hInst, HWND hwndParent, RECT rc);
	virtual ~BolotaView();

	// Document handling.
	void OpenDocument(Bolota::Document *doc);
	void CloseDocument();

	// Event handling.
	void ReloadView();
	void Resize(RECT rc) const;

	// Getters and setters.
	HWND WindowHandle() const;

	// Examples
	void OpenExampleDocument();

protected:
	// Tree-View item handling.
	HTREEITEM AddTreeViewItem(HTREEITEM htiParent, HTREEITEM htiInsertAfter,
		Bolota::Field *field);
};

#endif // _BOLOTA_BOLOTAVIEW_H
