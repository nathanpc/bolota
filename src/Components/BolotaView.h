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
#include "../FieldManagerDialog.h"

/**
* A TreeView component that displays and handles a Bolota document.
*/
class BolotaView {
private:
	// Data sources.
	Bolota::Document *m_doc;

	// Window handles.
	HINSTANCE m_hInst;
	HWND m_hWnd;
	HWND m_hwndParent;

public:
	// Constructors and destructors.
	BolotaView(HINSTANCE hInst, HWND hwndParent, RECT rc);
	virtual ~BolotaView();

	// Document handling.
	void OpenDocument(Bolota::Document *doc);
	void CloseDocument();

	// Event handling.
	LRESULT OpenFieldManager(FieldManagerDialog::DialogType type);
	bool Save(bool bSaveAs);
	bool OpenFile();
	void ReloadView();
	void Resize(RECT rc) const;

	// Getters and setters.
	Bolota::Field* GetSelectedField();
	Bolota::Field* GetSelectedField(HTREEITEM *htiSelected);
	HWND WindowHandle() const;

	// Examples
	void OpenExampleDocument();

protected:
	// Field updates.
	void RefreshField(HTREEITEM hti, Bolota::Field *field);
	void AppendField(HTREEITEM htiPrev, Bolota::Field *prev,
		Bolota::Field *field);
	void PrependField(HTREEITEM htiNext, Bolota::Field *next,
		Bolota::Field *field);
	void CreateChildField(HTREEITEM htiParent, Bolota::Field *parent,
		Bolota::Field *field);

	// Tree-View item handling.
	HTREEITEM AddTreeViewItem(HTREEITEM htiParent, HTREEITEM htiInsertAfter,
		Bolota::Field *field, bool bRecurse);
	HTREEITEM AddTreeViewItem(HTREEITEM htiParent, HTREEITEM htiInsertAfter,
		Bolota::Field *field);
	void SelectTreeViewItem(HTREEITEM hti);

	// Common dialogs.
	bool ShowFileDialog(LPTSTR szFilename, bool bSave);
};

#endif // _BOLOTA_BOLOTAVIEW_H
