/**
 * BolotaView.cpp
 * A TreeView component that displays and handles a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "BolotaView.h"

#include "../stdafx.h"
#include "../PropertiesDialog.h"
#ifdef DEBUG
	#include <string>
#endif // DEBUG

using namespace Bolota;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Constructs and adds the document viewer component to the parent window.
 *
 * @param hInst      Application instance.
 * @param hwndParent Parent window.
 * @param rc         Desired position and size of the component.
 */
BolotaView::BolotaView(HINSTANCE hInst, HWND hwndParent, RECT rc) {
	// Initialize some defaults.
	m_doc = NULL;
	m_hInst = hInst;
	m_hwndParent = hwndParent;

	// Create the window instance.
	m_hWnd = CreateWindowEx(
		0, WC_TREEVIEW, _T("BolotaView"),
		WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
		rc.left, rc.top, rc.right, rc.bottom,
		hwndParent, (HMENU)NULL, hInst, NULL);
	ShowWindow(m_hWnd, SW_SHOW);
}

/**
 * Destroys the window and all of its associated components.
 *
 * @warning This method will free the Bolota document object.
 */
BolotaView::~BolotaView() {
	// Destroy the window.
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;

	// Free the associated document object.
	CloseDocument();
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                            Document Handling                              |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Associates a document with the view and populates it.
 *
 * @param doc Document to be associated with the view. Will take ownership of
 *            it and control its allocation.
 */
void BolotaView::OpenDocument(Document *doc) {
	// Close the current document first and take ownership of the new one.
	CloseDocument();
	m_doc = doc;

	// Populate the view.
	ReloadView(doc->FirstTopic());
}

/**
 * Closes the associated document and clears the view.
 */
void BolotaView::CloseDocument() {
	// Should we do anything?
	if (m_doc == NULL)
		return;

	// Clear the Tree-View.
	TreeView_DeleteAllItems(m_hWnd);

	// Delete the document object.
	delete m_doc;
	m_doc = NULL;
}

/**
 * Opens an example document for testing.
 */
void BolotaView::OpenExampleDocument() {
	// Create example document.
	Document *doc = new Document(
		new TextField(_T("Example Document")),
		new TextField(_T("This is a test document for the Bolota application")),
		DateField::Now());

	// Populate example document.
	doc->AppendTopic(new TextField(_T("First top element")));
	TextField *tmpField = new TextField(_T("Second top element"));
	tmpField->SetChild(new TextField(
		_T("A sub-element of the second element")))
		->SetNext(new TextField(
		_T("Yet another sub-element of the second element")))
		->SetNext(DateField::Now());
	doc->AppendTopic(tmpField);
	tmpField = static_cast<TextField*>(
		tmpField->SetNext(new TextField(_T("Element 1")))
		->SetNext(new TextField(_T("Element 2"))));
	tmpField->SetNext(new TextField(_T("Third top element")))
		->SetChild(new TextField(_T("A sub-element of the third element")))
		->SetChild(new TextField(_T("A sub-sub-element of the third element")));
	doc->AppendTopic(new TextField(_T("Fourth top element")));

	// Open the example document.
	OpenDocument(doc);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                         Tree-View Item Handling                           |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Adds topic field items to the Tree-View.
 *
 * @param htiParent      Parent Tree-View item.
 * @param htiInsertAfter Tree-View item that will appear before this one.
 * @param field          Field to be added to the view.
 * @param bRecurse       Should we add child fields recursively?
 * @param bNext          Should we add next fields as well?
 * @param fldSelected    Optional. Which field should automatically be selected.
 *
 * @return Tree-View item root node that was just added.
 */
HTREEITEM BolotaView::AddTreeViewItem(HTREEITEM htiParent,
									  HTREEITEM htiInsertAfter, Field *field,
									  bool bRecurse, bool bNext,
									  Field *fldSelected) {
	// Build up the tree item object from the topic.
	TVITEM tvi;
	tvi.mask = TVIF_TEXT | TVIF_PARAM;
	tvi.pszText = const_cast<LPTSTR>(field->Text()->GetNativeString());
	tvi.lParam = reinterpret_cast<LPARAM>(field);

	// Create the Tree-View insertion object.
	TVINSERTSTRUCT tvins;
	tvins.item = tvi;
	tvins.hInsertAfter = htiInsertAfter;
	tvins.hParent = htiParent;

	// Insert the item in the Tree-View.
	HTREEITEM hti = TreeView_InsertItem(m_hWnd, &tvins);

	// Select the desired field if necessary.
	if (fldSelected && (field == fldSelected))
		SelectTreeViewItem(hti);

#ifdef DEBUG
	// Print out details about the added field for debugging.
	TCHAR szDebugMsg[256];
	_snwprintf(szDebugMsg, 255, _T("(%u) %s\r\n"), field->Depth(),
		field->Text()->GetNativeString());
	szDebugMsg[255] = _T('\0');
	OutputDebugString(szDebugMsg);
#endif // DEBUG

	// Go through child and next fields inserting them into the Tree-View as well.
	if (bRecurse && field->HasChild()) {
		AddTreeViewItem(hti, TVI_FIRST, field->Child(), bRecurse, true,
			fldSelected);
		TreeView_Expand(m_hWnd, hti, TVE_EXPAND);
	}

	// Insert next fields into the Tree-View as well.
	if (bNext && field->HasNext())
		AddTreeViewItem(htiParent, hti, field->Next(), true, true, fldSelected);

	return hti;
}

/**
 * Adds a single topic field item to the Tree-View.
 *
 * @param htiParent      Parent Tree-View item.
 * @param htiInsertAfter Tree-View item that will appear before this one.
 * @param field          Field to be added to the view.
 *
 * @return Tree-View item that was just added.
 */
HTREEITEM BolotaView::AddTreeViewItem(HTREEITEM htiParent,
									  HTREEITEM htiInsertAfter, Field *field) {
	return AddTreeViewItem(htiParent, htiInsertAfter, field, false, false,
		NULL);
}

/**
 * Selects and focuses on a Tree-View node.
 *
 * @param hti Tree-View item handle to be selected.
 */
void BolotaView::SelectTreeViewItem(HTREEITEM hti) {
	TreeView_SelectItem(m_hWnd, hti);
	SetFocus(m_hWnd);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                               Field Updates                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Refreshes a node in the Tree-View window with data from a field.
 *
 * @param hti   Tree-View node item to be updated.
 * @param field New field to be reloaded into the node.
 */
void BolotaView::RefreshField(HTREEITEM hti, Field *field) {
	// Refresh the node.
	TVITEM tvi;
	tvi.mask = TVIF_TEXT | TVIF_PARAM;
	tvi.hItem = hti;
	tvi.pszText = const_cast<LPTSTR>(field->Text()->GetNativeString());
	tvi.lParam = reinterpret_cast<LPARAM>(field);
	TreeView_SetItem(m_hWnd, &tvi);

	// Select the refreshed node and check the consistency of the tree.
	SelectTreeViewItem(hti);
	CheckTreeConsistency(hti);
}

/**
 * Appends a new field to the Tree-View.
 *
 * @param htiPrev Tree-View node item to be before the new one.
 * @param prev    Field before the one to be inserted.
 * @param field   New field to be inserted.
 */
void BolotaView::AppendField(HTREEITEM htiPrev, Bolota::Field *prev,
							 Field *field) {
	// Append the field in the document.
	m_doc->AppendTopic(prev, field);

	// Append the field to the Tree-View and select the new node.
	HTREEITEM htiParent = TreeView_GetParent(m_hWnd, htiPrev);
	if (htiParent == NULL)
		htiParent = TVI_ROOT;
	HTREEITEM hti = AddTreeViewItem(htiParent, htiPrev, field);
	SelectTreeViewItem(hti);

	// Check the consistency of the tree.
	CheckTreeConsistency(hti);
}

/**
 * Prepends a new field to the Tree-View.
 *
 * @param htiNext Tree-View node item to be after the new one.
 * @param next    Field after the one to be inserted.
 * @param field   New field to be inserted.
 */
void BolotaView::PrependField(HTREEITEM htiNext, Bolota::Field *next,
							 Field *field) {
	// Append the field in the document.
	m_doc->PrependTopic(next, field);

	// Prepend the field to the Tree-View and select the new node.
	HTREEITEM htiParent = TreeView_GetParent(m_hWnd, htiNext);
	if (htiParent == NULL)
		htiParent = TVI_ROOT;
	HTREEITEM htiPrev = TreeView_GetPrevSibling(m_hWnd, htiNext);
	if (htiPrev == NULL)
		htiPrev = TVI_FIRST;
	HTREEITEM hti = AddTreeViewItem(htiParent, htiPrev, field);
	SelectTreeViewItem(hti);

	// Check the consistency of the tree.
	CheckTreeConsistency(hti);
}

/**
 * Appends a new child field to the Tree-View.
 *
 * @param htiParent Tree-View node item to be the parent the new one.
 * @param parent    Parent field.
 * @param field     New field to be inserted.
 */
void BolotaView::CreateChildField(HTREEITEM htiParent, Bolota::Field *parent,
							 Field *field) {
	// Simply prepend to the first child if the parent already has one.
	if (parent->HasChild()) {
		PrependField(TreeView_GetChild(m_hWnd, htiParent), parent->Child(),
			field);
		return;
	}

	// Set the child field in the document and update the Tree-View.
	parent->SetChild(field, false);
	HTREEITEM hti = AddTreeViewItem(htiParent, TVI_FIRST, field);
	SelectTreeViewItem(hti);

	// Check the consistency of the tree.
	CheckTreeConsistency(hti);
}

/**
 * Checks if the internal field objects around a Tree-View item are all
 * consistent with eachother in their relationships.
 *
 * @param hti Tree-View item to check for consistency.
 *
 * @throws ConsistencyException if an inconsistency is found.
 */
void BolotaView::CheckTreeConsistency(HTREEITEM hti) {
	// Check if this is even valid.
	if (hti == NULL)
		throw std::exception("No Tree-View item passed for consistency check");

	// Get reference field.
	Field *ref = GetFieldFromTreeItem(hti);
	if (ref == NULL) {
		throw std::exception("Reference field for consistency check not found "
			"in Tree-View");
	}

	// Get parent field.
	HTREEITEM htiParent = TreeView_GetParent(m_hWnd, hti);
	Field *parent = (htiParent == NULL) ? NULL :
		GetFieldFromTreeItem(htiParent);

	// Get child field.
	HTREEITEM htiChild = TreeView_GetChild(m_hWnd, hti);
	Field *child = (htiChild == NULL) ? NULL :
		GetFieldFromTreeItem(htiChild);

	// Get previous field.
	HTREEITEM htiPrev = TreeView_GetPrevSibling(m_hWnd, hti);
	Field *prev = (htiPrev == NULL) ? NULL :
		GetFieldFromTreeItem(htiPrev);

	// Get next field.
	HTREEITEM htiNext = TreeView_GetNextSibling(m_hWnd, hti);
	Field *next = (htiNext == NULL) ? NULL :
		GetFieldFromTreeItem(htiNext);

	// Check our overall consistency.
	m_doc->CheckFieldConsistency(ref, parent, child, prev, next);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                              Event Handling                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Opens the appropriate field manager dialog window for the desired action.
 *
 * @param type Type of action to be performed in the manager dialog.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::OpenFieldManager(FieldManagerDialog::DialogType type) {
	// Get the currently selected field in the Tree-View.
	HTREEITEM hti = NULL;
	Field *field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Should we get a brand new field?
	Field *fldNew;
	switch (type) {
	case FieldManagerDialog::DialogType::AppendField:
	case FieldManagerDialog::DialogType::PrependField:
		fldNew = new TextField(field->Parent());
		break;
	case FieldManagerDialog::DialogType::NewChildField:
		fldNew = new TextField(field);
		break;
	default:
		fldNew = NULL;
	}

	// Setup and open the manager dialog.
	FieldManagerDialog dlgManager(this->m_hInst, this->m_hWnd, type,
		(fldNew) ? &fldNew : &field, field);
	INT_PTR iRet = dlgManager.ShowModal();

	// Check if the dialog returned from a Cancel operation.
	if (iRet == IDCANCEL) {
		// Clean up our unused field.
		if (fldNew) {
			delete fldNew;
			fldNew = NULL;
		}

		// Get the focus back on the component.
		SetFocus(m_hWnd);
		return IDCANCEL;
	}

	// Update the document viewer.
	switch (dlgManager.Type()) {
	case FieldManagerDialog::DialogType::EditField:
		RefreshField(hti, field);
		break;
	case FieldManagerDialog::DialogType::AppendField:
		AppendField(hti, field, fldNew);
		break;
	case FieldManagerDialog::DialogType::PrependField:
		PrependField(hti, field, fldNew);
		break;
	case FieldManagerDialog::DialogType::NewChildField:
		CreateChildField(hti, field, fldNew);
		break;
	default:
		MsgBoxError(this->m_hWnd, _T("Unknown operation type"), _T("Unable to ")
			_T("perform post-dialog operation on unknown operation type."));
	}

	return 0;
}

/**
 * Handles the deletion of the currently selected field item.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::AskDeleteField() {
	// Get the currently selected field in the Tree-View.
	HTREEITEM hti = NULL;
	Field *field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Create message for the question dialog.
	LPCTSTR szQuestion = (field->HasChild()) ?
		_T("Are you sure you want to delete this field and all its children?") :
		_T("Are you sure you want to delete this field?");
	LPTSTR szMsg = (LPTSTR)LocalAlloc(LMEM_FIXED,
		(_tcslen(szQuestion) + field->TextLength() + 4 + 1) * sizeof(TCHAR));
	if (szMsg == NULL) {
		MsgBoxError(m_hWnd, _T("Failed to allocate memory"),
			_T("Failed to allocate memory for the delete dialog message."));
		return 1;
	}
	_stprintf(szMsg, _T("%s\r\n\"%s\""), szQuestion,
		field->Text()->GetNativeString());

	// Ask the user if they are sure about this operation.
	int iRet = MsgBox(m_hWnd, MB_ICONQUESTION | MB_OKCANCEL, _T("Delete field"),
		szMsg);
	LocalFree(szMsg);
	szMsg = NULL;
	if (iRet != IDOK)
		return 0;

	// Get the next Tree-View item to be selected after we delete the field.
	HTREEITEM htiNext = TreeView_GetNextSibling(m_hWnd, hti);
	if (htiNext == NULL)
		TreeView_GetPrevSibling(m_hWnd, hti);

	// Delete the field and remove the item from the Tree-View.
	m_doc->DeleteTopic(field);
	TreeView_DeleteItem(m_hWnd, hti);
	if (htiNext != NULL) {
		SelectTreeViewItem(htiNext);
		CheckTreeConsistency(htiNext);
	}

	return 0;
}

/**
 * Moves a field up or down the tree.
 *
 * @param bUp Should we move the field upwards? Set to FALSE to move downwards.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::MoveField(bool bUp) {
	Field *fldAbove = NULL;

	// Get the currently selected field in the Tree-View.
	HTREEITEM hti = NULL;
	Field *field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Get the next visible item in the Tree-View.
	HTREEITEM htiAbove = NULL;
	if (bUp) {
		// Root field cannot be moved up so ignore it.
		if (m_doc->FirstTopic() == field)
			return 0;

		// To go up we need to get the item that's twice above us.
		htiAbove = TreeView_GetPrevVisible(m_hWnd,
			TreeView_GetPrevVisible(m_hWnd, hti));

		// Moving to the top of the tree.
		if (htiAbove == NULL) {
			m_doc->MoveTopicToTop(field);
			htiAbove = TVI_FIRST;
			goto refresh;
		}
	} else {
		// Last field cannot be moved down so ignore it.
		if (field->IsDocumentLast())
			return 0;

		if (!field->HasChild()) {
			htiAbove = TreeView_GetNextVisible(m_hWnd, hti);
		} else {
			// To go down we have to take our child with us.
			htiAbove = TreeView_GetNextSibling(m_hWnd, hti);

			// Handle last child of a node that has children of its own.
			HTREEITEM htiParent = TreeView_GetParent(m_hWnd, hti);
			while ((htiAbove == NULL) && (htiParent != NULL)) {
				htiAbove = TreeView_GetNextSibling(m_hWnd, htiParent);
				htiParent = TreeView_GetParent(m_hWnd, htiParent);
			}
		}
	}

	// Shuffle things around.
	fldAbove = GetFieldFromTreeItem(htiAbove);
	m_doc->MoveTopicBelow(field, fldAbove);

refresh:
	// Get the right parent for our Tree-View item.
	HTREEITEM htiParent = (htiAbove == TVI_FIRST) ? TVI_ROOT :
		TreeView_GetParent(m_hWnd, htiAbove);
	if (field->IsFirstChild()) {
		// Field became the first child of a topic.
		htiParent = htiAbove;
		htiAbove = TVI_FIRST;
	} else if (htiParent == NULL) {
		// Parent is the top-level.
		htiParent = TVI_ROOT;
	} 
	
	// Shuffle things around in the Tree-View.
	TreeView_DeleteItem(m_hWnd, hti);
	hti = AddTreeViewItem(htiParent, htiAbove, field, true, false, field);

	// Check the consistency of the tree.
	CheckTreeConsistency(hti);

	return 0;
}

/**
 * Indents a field relative to the one above it.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::IndentField() {
	// Get the currently selected field in the Tree-View.
	HTREEITEM hti = NULL;
	Field *field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Nothing can be done about top fields.
	if (!field->HasPrevious())
		return 0;

	// Indent the field internally.
	m_doc->IndentTopic(field);
	if (field->Parent() == field->Next())
		throw std::exception("Infinite loop was created when indenting field");

	// Get the right parent and previous items.
	HTREEITEM htiParent = TreeView_GetPrevSibling(m_hWnd, hti);
	HTREEITEM htiPrev = field->IsFirstChild() ? TVI_FIRST : TVI_LAST;

	// Indent the field in the Tree-View.
	TreeView_DeleteItem(m_hWnd, hti);
	hti = AddTreeViewItem(htiParent, htiPrev, field, true, false, field);

	// Check the consistency of the tree.
	CheckTreeConsistency(TreeView_GetSelection(m_hWnd));

	return 0;
}

/**
 * De-indents a field relative to its parent.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::DeindentField() {
	// Get the currently selected field in the Tree-View.
	HTREEITEM hti = NULL;
	Field *field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Nothing can be done about orphan fields.
	if (!field->HasParent())
		return 0;
	
	// Deindent the field internally.
	m_doc->DeindentTopic(field);

	// Get the right parent and previous items.
	HTREEITEM htiPrev = TreeView_GetParent(m_hWnd, hti);
	HTREEITEM htiParent = !field->HasParent() ? TVI_ROOT :
		TreeView_GetParent(m_hWnd, htiPrev);

	// Deindent the field in the Tree-View.
	TreeView_DeleteItem(m_hWnd, hti);
	hti = AddTreeViewItem(htiParent, htiPrev, field, true, false, field);

	// Check the consistency of the tree.
	CheckTreeConsistency(TreeView_GetSelection(m_hWnd));

	return 0;
}

/**
 * Tries to save the current document to a file.
 *
 * @param bSaveAs Should we perform the default for a Save As operation?
 *
 * @return TRUE if everything worked, FALSE otherwise.
 */
bool BolotaView::Save(bool bSaveAs) {
	TCHAR szFilename[MAX_PATH];
	szFilename[0] = _T('\0');

	// Check if we are just doing an incremental save.
	if (m_doc->HasFileAssociated()) {
		if (bSaveAs) {
			_tcscpy(szFilename, m_doc->FilePath().GetNativeString());
		} else {
			m_doc->WriteFile();
			return true;
		}
	}

	// Show the save file dialog and process the user selection.
	if (!ShowFileDialog(szFilename, true))
		return true;
	try {
		// Write the file and set the window title.
		m_doc->WriteFile(szFilename, true);
		SetWindowText(m_hwndParent, PathFindFileName(szFilename));
	} catch (std::exception& e) {
		MsgBoxException(m_hwndParent, e, _T("Cannot save document"));
	}

	return true;
}

/**
 * Tries to open a new document from a file.
 *
 * @return TRUE if everything worked, FALSE otherwise.
 */
bool BolotaView::OpenFile() {
	TCHAR szFilename[MAX_PATH];
	szFilename[0] = _T('\0');

	// Show the open file dialog and process the user selection.
	if (!ShowFileDialog(szFilename, false))
		return true;
	try {
		// Open the document and set the window title.
		OpenDocument(Document::ReadFile(szFilename));
		SetWindowText(m_hwndParent, PathFindFileName(szFilename));
	} catch (std::exception& e) {
		MsgBoxException(m_hwndParent, e, _T("Cannot open document"));
	}

	return true;
}

/**
 * Sets up an open/save file dialog structure.
 *
 * @param szFilename Pointer to store (and retrieve) the path to the file.
 * @param bSave      Is this a save file dialog?
 *
 * @return TRUE if the operation was successful. FALSE if the user canceled or
 *         an error occurred.
 */
bool BolotaView::ShowFileDialog(LPTSTR szFilename, bool bSave) {
	// Setup file dialog.
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hwndParent;
	ofn.lpstrTitle = (bSave) ? _T("Save Document As...") : _T("Open Document");
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.Flags |= (bSave) ? OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT :
		OFN_FILEMUSTEXIST;
	ofn.lpstrFilter = _T("Bolota Documents (*.bol)\0*.bol\0")
		_T("All Files (*.*)\0*.*\0");
	ofn.lpstrFile = szFilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = _T("bol");

	// Open the common file dialog.
	if (bSave)
		return GetSaveFileName(&ofn) != 0;
	return GetOpenFileName(&ofn) != 0;
}

/**
 * Opens the document properties editor dialog window and performs any updates
 * to the associated document.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::EditProperties() {
	// Setup and open the properties editor dialog.
	PropertiesDialog dlgEditor(this->m_hInst, this->m_hWnd, m_doc);
	INT_PTR iRet = dlgEditor.ShowModal();

	// Get the focus back on the component and return.
	SetFocus(m_hWnd);
	return 0;
}

/**
 * Completely reloads the Tree-View component with data from the opened
 * document.
 *
 * @param fldSelected Optional. Which field should automatically be selected.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::ReloadView(Field *fldSelected) {
	// Clear the Tree-View for good measure.
	TreeView_DeleteAllItems(m_hWnd);

	// Handle empty documents.
	if (m_doc->IsEmpty())
		// TODO: Show a nice label with a message about creating a new topic.
		return 0;

	// Populate the Tree-View with topics.
	AddTreeViewItem(TVI_ROOT, TVI_FIRST, m_doc->FirstTopic(), true, true,
		fldSelected);

	return 0;
}

/**
 * Completely reloads the Tree-View component with data from the opened
 * document.
 *
 * @param fldSelected Optional. Which field should automatically be selected.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::ReloadView() {
	return ReloadView(NULL);
}

/**
 * Resizes the component window.
 *
 * @param rc Desired position and size of the component.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::Resize(RECT rc) const {
	SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right, rc.bottom,
		SWP_NOZORDER);
	return 0;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the associated field object from a Tree-View item.
 *
 * @param hti Tree-View item handle.
 *
 * @return Field object associated with the Tree-View item.
 */
Field* BolotaView::GetFieldFromTreeItem(HTREEITEM hti) const {
	// Check if we have a valid item handle.
	if (hti == NULL)
		throw std::exception("Tree-View item for field retrieval is NULL");

	// Get the selected item from the handle.
	TVITEM tvi;
	tvi.hItem = hti;
	tvi.mask = TVIF_PARAM;
	if (!TreeView_GetItem(m_hWnd, &tvi))
		throw std::exception("Failed to get Tree-View item from handle");

	return reinterpret_cast<Field*>(tvi.lParam);
}

/**
 * Gets the currently selected item in the viewer.
 *
 * @param htiSelected Returns selected Tree-View item handle. Set to NULL if it
 *                    should be ignored.
 * @param bShowError  Should we show an error dialog if no field is selected?
 *
 * @return Currently selected field or NULL if none are selected.
 */
Field* BolotaView::GetSelectedField(HTREEITEM *htiSelected,
									bool bShowError) const {
	// Get the currently selected item handle in the Tree-View.
	HTREEITEM hti = TreeView_GetSelection(m_hWnd);
	if (hti == NULL) {
		if (bShowError) {
			MsgBoxError(this->m_hwndParent, _T("No field selected"),
				_T("In order to perform this operation a field must be ")
				_T("selected."));
		}
		return NULL;
	}

	// Return item handle.
	if (htiSelected != NULL)
		*htiSelected = hti;

	return GetFieldFromTreeItem(hti);
}

/**
 * Gets the currently selected item in the viewer.
 *
 * @param bShowError Should we show an error dialog if no field is selected?
 *
 * @return Currently selected field or NULL if none are selected.
 */
Field* BolotaView::GetSelectedField(bool bShowError) const {
	return GetSelectedField(NULL, bShowError);
}

/**
 * Gets the component's window handle.
 *
 * @return Associated window handle.
 */
HWND BolotaView::WindowHandle() const {
	return m_hWnd;
}
