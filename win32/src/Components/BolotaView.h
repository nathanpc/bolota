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

#include "../../Bolota/Document.h"
#include "../FieldManagerDialog.h"
#include "FieldImageList.h"

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
	HMENU m_hmnuParent;

	// Image lists.
	FieldImageList *m_imlFieldIcons;

public:
	// Constructors and destructors.
	BolotaView(HINSTANCE hInst, HWND hwndParent, HMENU hMenu, RECT rc);
	virtual ~BolotaView();

	// Document handling.
	void NewDocument();
	bool OpenDocument(Bolota::Document *doc);
	bool OpenDocument(LPCTSTR szFilename);
	void CloseDocument();

	// Event handling.
	LRESULT OpenFieldManager(FieldManagerDialog::DialogType type);
	LRESULT AskDeleteField();
	LRESULT MoveField(bool bUp);
	LRESULT IndentField();
	LRESULT DeindentField();
	LRESULT AppendToParent();
	bool Save(bool bSaveAs);
	bool OpenFile();
	bool Close();
	LRESULT EditProperties();
	LRESULT ReloadView();
	LRESULT ReloadView(Bolota::Field *fldSelected);
	LRESULT Resize(RECT rc) const;
	bool ShowContextMenu(int xPos, int yPos);

	// Getters and setters.
	void SetDirty(bool bDirty);
	bool IsDirty() const;
	Bolota::Field* GetFieldFromTreeItem(HTREEITEM hti) const;
	Bolota::Field* GetSelectedField(bool bShowError) const;
	Bolota::Field* GetSelectedField(HTREEITEM *htiSelected,
		bool bShowError) const;
	HWND WindowHandle() const;

	// Examples
	void OpenExampleDocument();

	// Settings
	LRESULT ViewSettingUpdate(UINT_PTR uMenuId);
	bool UpdateViewFromSettings() const;

protected:
	// Field updates.
	bool RefreshField(HTREEITEM hti, Bolota::Field *field);
	bool AppendField(HTREEITEM htiPrev, Bolota::Field *prev,
		Bolota::Field *field);
	bool PrependField(HTREEITEM htiNext, Bolota::Field *next,
		Bolota::Field *field);
	bool CreateChildField(HTREEITEM htiParent, Bolota::Field *parent,
		Bolota::Field *field);
	bool CheckTreeConsistency(HTREEITEM hti);

	// Tree-View item handling.
	HTREEITEM AddTreeViewItem(HTREEITEM htiParent, HTREEITEM htiInsertAfter,
		Bolota::Field *field, bool bRecurse, bool bNext,
		Bolota::Field *fldSelected);
	HTREEITEM AddTreeViewItem(HTREEITEM htiParent, HTREEITEM htiInsertAfter,
		Bolota::Field *field);
	void SelectTreeViewItem(HTREEITEM hti);

	// Helpers
	LPTSTR FieldDisplayText(Bolota::Field *field, bool *bRetain) const;
	LPTSTR SetTreeViewItemField(LPTVITEM lptvi, Bolota::Field *field,
		bool *bRetain) const;
	LPTSTR GetFilename(LPCTSTR szFilepath) const;
	bool ShowFileDialog(LPTSTR szFilename, bool bSave) const;
};

#endif // _BOLOTA_BOLOTAVIEW_H
