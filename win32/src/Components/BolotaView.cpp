/**
 * BolotaView.cpp
 * A TreeView component that displays and handles a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "BolotaView.h"

#include "../stdafx.h"
#include "../PropertiesDialog.h"
#include "../../Bolota/Errors/Error.h"
#include "../Utilities/Settings/ConfigManager.h"

#ifndef UNDER_CE
	#include <shlwapi.h>
#endif // !UNDER_CE

// Zero-based index of the position of the Field menu in the parent's menu bar.
#define PARENT_MENU_FIELD_IDX 1

using namespace Bolota;
using namespace Settings;

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
 * @param hMenu      Application's main menu handle.
 * @param rc         Desired position and size of the component.
 */
BolotaView::BolotaView(HINSTANCE hInst, HWND hwndParent, HMENU hMenu, RECT rc) {
	// Initialize some defaults.
	m_doc = NULL;
	m_hInst = hInst;
	m_hwndParent = hwndParent;
	m_hmnuParent = hMenu;

	// Initialize ImageLists.
	m_imlFieldIcons = new FieldImageList(hInst);

	// Create the window instance.
	m_hWnd = CreateWindowEx(0, WC_TREEVIEW, _T("BolotaView"), WS_VISIBLE |
		WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
		rc.left, rc.top, rc.right, rc.bottom, hwndParent, (HMENU)NULL, hInst,
		NULL);
	UpdateViewFromSettings();
	ShowWindow(m_hWnd, SW_SHOW);

	// Set the ImageList associated with the TreeView.
	TreeView_SetImageList(m_hWnd, m_imlFieldIcons->Handle(), TVSIL_NORMAL);
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

	// Destroy ImageLists.
	if (m_imlFieldIcons)
		delete m_imlFieldIcons;
	m_imlFieldIcons = NULL;

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
 * Creates a brand new blank document for the user to start from a clean slate.
 */
void BolotaView::NewDocument() {
	// Prevent the user from doing something stupid.
	if (!Close())
		return;

	// Close the current document first and create a new one.
	CloseDocument();
	m_doc = new Document(new TextField(_T("")), new TextField(_T("")),
		DateField::Now());

	// Populate the view and show the properties dialog for initial changes.
	ReloadView();
	SetDirty(false);
	EditProperties();
}

/**
 * Associates a document with the view and populates it.
 *
 * @param doc Document to be associated with the view. Will take ownership of
 *            it and control its allocation.
 * 
 * @return TRUE if the operation was successful. FALSE otherwise.
 */
bool BolotaView::OpenDocument(Document *doc) {
	// Prevent the user from doing something stupid.
	if (!Close())
		return true;

	// Close the current document first and take ownership of the new one.
	CloseDocument();
	m_doc = doc;

	// Populate the view and flag saved changes
	ReloadView(doc->FirstTopic());
	SetDirty(false);

	return true;
}

/**
 * Associates a document with the view and populates it.
 *
 * @param doc Document to be associated with the view. Will take ownership of
 *            it and control its allocation.
 * 
 * @return TRUE if the operation was successful. FALSE otherwise.
 */
bool BolotaView::OpenDocument(LPCTSTR szFilename) {
	// Prevent the user from doing something stupid.
	if (!Close())
		return true;

	// Read the file into a document object.
	Document* doc = Document::ReadFile(szFilename);
	if (doc == BOLOTA_ERR_NULL) {
		MsgBoxBolotaError(m_hwndParent, _T("Cannot open document"));
		return false;
	}

	// Update the Tree-View with the contents of the document.
	OpenDocument(doc);

	// Set the window title to match the filename.
#ifndef UNDER_CE
	SetWindowText(m_hwndParent, PathFindFileName(szFilename));
#else
	SHFILEINFO sfi = { 0 };
	SHGetFileInfo(szFilename, -1, &sfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
	SetWindowText(m_hwndParent, sfi.szDisplayName);
#endif // !UNDER_CE

	// Flag saved changes.
	SetDirty(false);

	return true;
}

/**
 * Closes the associated document and clears the view.
 */
void BolotaView::CloseDocument() {
	// Should we do anything?
	if (m_doc == NULL)
		return;

	// Prevent the user from doing something stupid.
	if (!Close())
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
	DateField *fldDate = DateField::Now();
	fldDate->SetText(_T("An example of a date time field."));
	tmpField->SetChild(new TextField(
		_T("A sub-element of the second element")))
		->SetNext(new IconField(BOLOTA_ICON_CPU,
		_T("Bolota is the best way to take notes.")))
		->SetNext(new TextField(
		_T("Yet another sub-element of the second element")))
		->SetNext(fldDate);
	doc->AppendTopic(tmpField);
	tmpField = static_cast<TextField*>(
		tmpField->SetNext(new TextField(_T("Element 1")))
		->SetNext(new TextField(_T("Element 2"))));
	tmpField->SetNext(new TextField(_T("Third top element")))
		->SetChild(new TextField(_T("A sub-element of the third element")))
		->SetChild(new TextField(_T("A sub-sub-element of the third element")));
	doc->AppendTopic(new BlankField());
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
	bool bRetain = false;
	LPTSTR szText = SetTreeViewItemField(&tvi, field, &bRetain);

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
	_snwprintf(szDebugMsg, 255, _T("(%u) %s\r\n"), field->Depth(), szText);
	szDebugMsg[255] = _T('\0');
	OutputDebugString(szDebugMsg);
#endif // DEBUG

	// Release the display text if needed.
	if (!bRetain && (field->Type() != BOLOTA_TYPE_BLANK)) {
		free(szText);
		szText = NULL;
	}

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
 *
 * @return FALSE if an error was found, TRUE if everything is fine.
 */
bool BolotaView::RefreshField(HTREEITEM hti, Field *field) {
	// Setup new node information.
	TVITEM tvi;
	bool bRetain = false;
	LPTSTR szText = SetTreeViewItemField(&tvi, field, &bRetain);
	tvi.hItem = hti;

	// Update the node.
	TreeView_SetItem(m_hWnd, &tvi);

	// Release the display text if needed.
	if (!bRetain && (field->Type() != BOLOTA_TYPE_BLANK)) {
		free(szText);
		szText = NULL;
	}

	// Select the refreshed node and check the consistency of the tree.
	SelectTreeViewItem(hti);
	return CheckTreeConsistency(hti);
}

/**
 * Appends a new field to the Tree-View.
 *
 * @param htiPrev Tree-View node item to be before the new one.
 * @param prev    Field before the one to be inserted.
 * @param field   New field to be inserted.
 *
 * @return FALSE if an error was found, TRUE if everything is fine.
 */
bool BolotaView::AppendField(HTREEITEM htiPrev, Bolota::Field *prev,
							 Field *field) {
	// Append the field in the document.
	if (!m_doc->AppendTopic(prev, field))
		return false;

	// Append the field to the Tree-View and select the new node.
	HTREEITEM htiParent = TreeView_GetParent(m_hWnd, htiPrev);
	if (htiParent == NULL)
		htiParent = TVI_ROOT;
	HTREEITEM hti = AddTreeViewItem(htiParent, htiPrev, field);
	SelectTreeViewItem(hti);

	// Flag unsaved changes.
	SetDirty(true);

	// Check the consistency of the tree.
	return CheckTreeConsistency(hti);
}

/**
 * Prepends a new field to the Tree-View.
 *
 * @param htiNext Tree-View node item to be after the new one.
 * @param next    Field after the one to be inserted.
 * @param field   New field to be inserted.
 *
 * @return FALSE if an error was found, TRUE if everything is fine.
 */
bool BolotaView::PrependField(HTREEITEM htiNext, Bolota::Field *next,
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

	// Flag unsaved changes.
	SetDirty(true);

	// Check the consistency of the tree.
	return CheckTreeConsistency(hti);
}

/**
 * Appends a new child field to the Tree-View.
 *
 * @param htiParent Tree-View node item to be the parent the new one.
 * @param parent    Parent field.
 * @param field     New field to be inserted.
 *
 * @return FALSE if an error was found, TRUE if everything is fine.
 */
bool BolotaView::CreateChildField(HTREEITEM htiParent, Bolota::Field *parent,
								  Field *field) {
	// Simply prepend to the first child if the parent already has one.
	if (parent->HasChild()) {
		PrependField(TreeView_GetChild(m_hWnd, htiParent), parent->Child(),
			field);
		return false;
	}

	// Set the child field in the document and update the Tree-View.
	parent->SetChild(field, false);
	HTREEITEM hti = AddTreeViewItem(htiParent, TVI_FIRST, field);
	SelectTreeViewItem(hti);

	// Flag unsaved changes.
	SetDirty(true);

	// Check the consistency of the tree.
	return CheckTreeConsistency(hti);
}

/**
 * Checks if the internal field objects around a Tree-View item are all
 * consistent with eachother in their relationships.
 *
 * @param hti Tree-View item to check for consistency.
 *
 * @return FALSE if an inconsistency is found, TRUE if everything is fine.
 */
bool BolotaView::CheckTreeConsistency(HTREEITEM hti) {
	// Check if this is even valid.
	if (hti == NULL) {
		ThrowError(EMSG("No Tree-View item passed for consistency check"));
		return false;
	}

	// Get reference field.
	Field *ref = GetFieldFromTreeItem(hti);
	if (ref == NULL) {
		ThrowError(EMSG("Reference field for consistency check not found ")
			_T("in Tree-View"));
		return false;
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
	return m_doc->CheckFieldConsistency(ref, parent, child, prev, next) == NULL;
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
 * @attention Check for errors with BolotaHasError after calling this method.
 *
 * @param type Type of action to be performed in the manager dialog.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::OpenFieldManager(FieldManagerDialog::DialogType type) {
	HTREEITEM hti = NULL;
	Field *field = NULL;

	// Handle the creation of the first field.
	if (m_doc->IsEmpty()) {
		m_doc->AppendTopic(new TextField(_T("")));
		ReloadView(m_doc->FirstTopic());
		type = FieldManagerDialog::DialogType::EditField;
	}

	// Get the currently selected field in the Tree-View.
	field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;
	bool bFirstTopic = field == m_doc->FirstTopic();

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
	FieldManagerDialog dlgManager(this->m_hInst, this->m_hWnd, m_imlFieldIcons,
		type, (fldNew) ? &fldNew : &field, field);
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

	// Flag unsaved changes.
	SetDirty(true);

	// Ensure that we update the first topic if it was replaced.
	if (bFirstTopic && (m_doc->FirstTopic() != field))
		m_doc->SetFirstTopic(field);

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

	// Check if anything bad happened.
	if (BolotaHasError) {
		MsgBoxBolotaError(this->m_hwndParent, _T("Failed to perform field operation"));
		return 1;
	}

	return 0;
}

/**
 * Handles the deletion of the currently selected field item.
 * 
 * @attention Check for errors with BolotaHasError after calling this method.
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
		return 1;
	}

	// Flag unsaved changes.
	SetDirty(true);

	return 0;
}

/**
 * Moves a field up or down the tree.
 * 
 * @attention Check for errors with BolotaHasError after calling this method.
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
	if (fldAbove == NULL) {
		ThrowError(EMSG("Failed to get above field from Tree-View"));
		return 1;
	}
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
	if (BolotaHasError)
		return 1;

	// Flag unsaved changes.
	SetDirty(true);

	return 0;
}

/**
 * Indents a field relative to the one above it.
 * 
 * @attention Check for errors with BolotaHasError after calling this method.
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
	if (BolotaHasError)
		return 1;
	if (field->Parent() == field->Next()) {
		ThrowError(EMSG("Infinite loop was created when indenting field"));
		return 1;
	}

	// Get the right parent and previous items.
	HTREEITEM htiParent = TreeView_GetPrevSibling(m_hWnd, hti);
	HTREEITEM htiPrev = field->IsFirstChild() ? TVI_FIRST : TVI_LAST;

	// Indent the field in the Tree-View.
	TreeView_DeleteItem(m_hWnd, hti);
	hti = AddTreeViewItem(htiParent, htiPrev, field, true, false, field);

	// Check the consistency of the tree.
	CheckTreeConsistency(TreeView_GetSelection(m_hWnd));
	if (BolotaHasError)
		return 1;

	// Flag unsaved changes.
	SetDirty(true);

	return 0;
}

/**
 * De-indents a field relative to its parent.
 * 
 * @attention Check for errors with BolotaHasError after calling this method.
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
	if (BolotaHasError)
		return 1;

	// Get the right parent and previous items.
	HTREEITEM htiPrev = TreeView_GetParent(m_hWnd, hti);
	HTREEITEM htiParent = !field->HasParent() ? TVI_ROOT :
		TreeView_GetParent(m_hWnd, htiPrev);

	// Deindent the field in the Tree-View.
	TreeView_DeleteItem(m_hWnd, hti);
	hti = AddTreeViewItem(htiParent, htiPrev, field, true, false, field);

	// Check the consistency of the tree.
	CheckTreeConsistency(TreeView_GetSelection(m_hWnd));
	if (BolotaHasError)
		return 1;

	// Flag unsaved changes.
	SetDirty(true);

	return 0;
}

/**
 * Appends a field to the currently selected field's parent or simply appends a
 * field if no parent exists.
 *
 * @attention Check for errors with BolotaHasError after calling this method.
 *
 * @return 0 if everything worked.
 */
LRESULT BolotaView::AppendToParent() {
	HTREEITEM hti = NULL;
	Field* field = NULL;

	// Handle the first field.
	if (m_doc->IsEmpty())
		goto append;

	// Get the currently selected field in the Tree-View.
	field = GetSelectedField(&hti, true);
	if (field == NULL)
		return 1;

	// Simply append if no parent is available.
	if (!field->HasParent())
		goto append;

	// Select the parent Tree-View item.
	hti = TreeView_GetParent(this->m_hWnd, hti);
	if (hti == NULL) {
		ThrowError(EMSG("Failed to get parent Tree-View item to append to"));
		return 1;
	}
	SelectTreeViewItem(hti);

append:
	return OpenFieldManager(FieldManagerDialog::AppendField);
}

/**
 * Tries to save the current document to a file.
 *
 * @param bSaveAs Should we perform the default for a Save As operation?
 *
 * @return TRUE if the file was saved. FALSE if an error occurred or the
 *         operation was cancelled.
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
			if (BolotaHasError) {
				MsgBoxBolotaError(m_hwndParent, _T("Cannot save document"));
				return false;
			}
			SetDirty(false);
			return true;
		}
	} else {
		// Create a friendlier default file name.
		timestamp_t ts = m_doc->Date()->Timestamp();
		if (!m_doc->Title()->Text()->Empty()) {
			_stprintf(szFilename, _T("%04u-%02u-%02u_%s"), ts.year, ts.month,
				ts.day, m_doc->Title()->Text()->GetNativeString());
		} else {
			_stprintf(szFilename, _T("%04u-%02u-%02u"), ts.year, ts.month,
				ts.day);
		}
	}

	// Show the save file dialog and process the user selection.
	if (!ShowFileDialog(szFilename, true))
		return false;

	// Write the file.
	m_doc->WriteFile(szFilename, true);
	if (BolotaHasError) {
		MsgBoxBolotaError(m_hwndParent, _T("Cannot save document"));
		return false;
	}

	// Set the window title.
	LPTSTR szBasename = GetFilename(szFilename);
	SetWindowText(m_hwndParent, szBasename);
	free(szBasename);
	szBasename = NULL;

	// Flag saved changes.
	SetDirty(false);

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

	return OpenDocument(szFilename);
}

/**
 * Handles a request to close the document, allowing us to prevent a close
 * operation in the case of unsaved changes.
 *
 * @return TRUE if we should allow the document to be closed. FALSE otherwise.
 */
bool BolotaView::Close() {
	// Should we do anything?
	if (m_doc == NULL)
		return true;

	// Check if we have unsaved changes and let the user decide what to do.
	if (IsDirty()) {
		int iAnswer = MsgBox(this->m_hwndParent, MB_YESNOCANCEL |
			MB_ICONQUESTION, _T("Unsaved changes"),
			_T("You have unsaved changes. Do you want to save them?"));

		// Stop everything if the user selected Cancel.
		if (iAnswer == IDCANCEL)
			return false;

		// Present a save dialog if the user selected Yes.
		if (iAnswer == IDYES) {
			if (!Save(false))
				return Close();
		}
	}

	// Also make it clean just for good measure.
	m_doc->SetDirty(false);

	return true;
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

	// Flag unsaved changes.
	SetDirty(m_doc->IsDirty());

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
		return 0;

	// Populate the Tree-View with topics.
	AddTreeViewItem(TVI_ROOT, TVI_FIRST, m_doc->FirstTopic(), true, true,
		fldSelected);

	// Flag unsaved changes.
	SetDirty(m_doc->IsDirty());

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

/**
 * Handles the Tree-View being right clicked.
 *
 * @param xPos X position of the right-click in screen coordinates.
 * @param yPos Y position of the right-click in screen coordinates.
 *
 * @return TRUE if the right click hit an item. FALSE otherwise.
 */
bool BolotaView::ShowContextMenu(int xPos, int yPos) {
	TVHITTESTINFO tvhti = {0};
	RECT rc;

	// Convert screen coordinates to client coordinates and populate hit test.
	GetWindowRect(m_hWnd, &rc);
	tvhti.pt.x = xPos - rc.left;
	tvhti.pt.y = yPos - rc.top;

	// Check if any items were hit by the right click.
	HTREEITEM hti = TreeView_HitTest(m_hWnd, &tvhti);
	if (hti == NULL)
		return false;

	// Select the right clicked item just to be sure.
	TreeView_SelectItem(m_hWnd, hti);

	// Get the menu we want to use as our context menu and show it.
#ifndef UNDER_CE
	HMENU hm = GetSubMenu(GetMenu(m_hwndParent), PARENT_MENU_FIELD_IDX);
	TrackPopupMenu(hm, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
		xPos, yPos, 0, m_hwndParent, NULL);
#else
	MsgBoxWarning(m_hwndParent, _T("Not yet implemented"),
		_T("Context menus are not yet implemented under Windows CE."));
#endif // !UNDER_CE

	return true;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                 Helpers                                   |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets a properly formatted string for displaying the specified field.
 *
 * @warning This method allocates memory which may need to be freed by you if
 *          bRetain is FALSE.
 *
 * @param field   Field to be displayed.
 * @param bRetain Optional. Returns if we should NOT free the text after it's
 *                returned and we use it.
 *
 * @return Properly formatted string for displaying the contents of the field.
 */
LPTSTR BolotaView::FieldDisplayText(Field *field, bool *bRetain) const {
	LPTSTR szText = const_cast<LPTSTR>(field->Text()->GetNativeString());
	size_t ulLength = 22 + 1 + _tcslen(szText);
	if (bRetain)
		*bRetain = false;

	switch (field->Type()) {
	case BOLOTA_TYPE_DATE: {
		// Get timestamp.
		DateField *fldDate = static_cast<DateField*>(field);
		timestamp_t ts = fldDate->Timestamp();

		// Build the display string.
		szText = (LPTSTR)malloc(ulLength * sizeof(TCHAR));
		szText[ulLength - 1] = _T('\0');
		_sntprintf(szText, ulLength - 1, _T("(%04u-%02u-%02u %02u:%02u:%02u) ")
			_T("%s"), ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second,
			field->Text()->GetNativeString());
		break;
	}
	default:
		if (bRetain)
			*bRetain = true;
	}

	return szText;
}

/**
 * Sets up a Tree-View item to be associated with a field. This will change the
 * entire Tree-View item to have the information associated with the field.
 *
 * @warning This method allocates memory which may need to be freed by you if
 *          bRetain is FALSE.
 *
 * @param lptvi   Pointer to the TVITEM structure to change.
 * @param field   Associated field.
 * @param bRetain Returns if we should NOT free the text after it's returned and
 *                we use it.
 *
 * @return Text used in the Tree-View item. Must be free'd by you if bRetain is
 *         FALSE.
 */
LPTSTR BolotaView::SetTreeViewItemField(LPTVITEM lptvi, Field *field,
										bool *bRetain) const {
	// Get the proper display string.
	LPTSTR szText = (field->Type() == BOLOTA_TYPE_BLANK) ? (LPTSTR)_T("") :
		FieldDisplayText(field, bRetain);

	lptvi->mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	lptvi->iImage = m_imlFieldIcons->Bullet();
	lptvi->iSelectedImage = m_imlFieldIcons->Bullet();
	lptvi->pszText = szText;
	lptvi->lParam = reinterpret_cast<LPARAM>(field);

	// Handle fields with icons.
	switch (field->Type()) {
	case BOLOTA_TYPE_ICON: {
		field_icon_t fiIndex = static_cast<IconField*>(field)->IconIndex();
		lptvi->iImage = m_imlFieldIcons->IndexFromFieldIndex(fiIndex);
		lptvi->iSelectedImage = m_imlFieldIcons->IndexFromFieldIndex(fiIndex);
		break;
	}
	case BOLOTA_TYPE_DATE:
		lptvi->iImage = m_imlFieldIcons->Calendar();
		lptvi->iSelectedImage = m_imlFieldIcons->Calendar();
		break;
	case BOLOTA_TYPE_BLANK:
		lptvi->iImage = m_imlFieldIcons->Blank();
		lptvi->iSelectedImage = m_imlFieldIcons->Blank();
		break;
	}

	return szText;
}


/**
 * Gets the basename of a filepath (aka just the file name).
 *
 * @warning This method allocates memory dynamically.
 *
 * @param szFilepath Full path to a file.
 *
 * @return Newly allocated string with just the filename in it.
 */
LPTSTR BolotaView::GetFilename(LPCTSTR szFilepath) const {
	LPTSTR szFilename;

#ifdef UNDER_CE
	SHFILEINFO sfi = {0};
	SHGetFileInfo(szFilepath, -1, &sfi, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME);
	szFilename = _tcsdup(sfi.szDisplayName);
#else
	szFilename = _tcsdup(PathFindFileName(szFilepath));
#endif // UNDER_CE

	return szFilename;
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
bool BolotaView::ShowFileDialog(LPTSTR szFilename, bool bSave) const {
	// Setup file dialog.
	OPENFILENAME ofn = {0};
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

/*
 * +===========================================================================+
 * |                                                                           |
 * |                                Settings                                   |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Updates the view settings of our document viewer.
 * 
 * @param uMenuId ID of the menu item that was clicked.
 * 
 * @return 0 if everything worked.
 */
LRESULT BolotaView::ViewSettingUpdate(UINT_PTR uMenuId) {
	// Get style flags from settings.
	DWORD dwStyle = Settings_GetValue(DWORD, ConfigManager::TreeViewStyleFlags);

	// Update the flags accordingly.
	UINT uState = 0;
	switch (uMenuId) {
	case IDM_VIEW_SHOWSUBLINES:
		dwStyle ^= TVS_HASLINES;
		break;
	case IDM_VIEW_SHOWROOTLINES:
		dwStyle ^= TVS_LINESATROOT;
		break;
	case IDM_VIEW_SHOWBUTTONS:
		dwStyle ^= TVS_HASBUTTONS;
		break;
	}

	// Update the application's settings.
	Settings_SaveValue(DWORD, ConfigManager::TreeViewStyleFlags, dwStyle);

	// Update the view and menu items.
	if (!UpdateViewFromSettings())
		return 1;

	return 0;
}

/**
 * Updates the style of the Tree-View and menu items according to the associated
 * styling flags setting.
 * 
 * @return TRUE if the operation was successful, FALSE otherwise.
 */
bool BolotaView::UpdateViewFromSettings() const {
	// Get style flags from settings.
	LONG_PTR dwStyle = WS_VISIBLE | WS_CHILD | Settings_GetValue(DWORD,
		ConfigManager::TreeViewStyleFlags);

	// Set the checkboxes inside the View menu.
	CheckMenuItem(m_hmnuParent, IDM_VIEW_SHOWSUBLINES,
		(dwStyle & TVS_HASLINES) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hmnuParent, IDM_VIEW_SHOWROOTLINES,
		(dwStyle & TVS_LINESATROOT) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(m_hmnuParent, IDM_VIEW_SHOWBUTTONS,
		(dwStyle & TVS_HASBUTTONS) ? MF_CHECKED : MF_UNCHECKED);

	// Set the style of the Tree-View.
	if (!SetWindowLongPtr(this->m_hWnd, GWL_STYLE, dwStyle)) {
		ThrowError(new SystemError(
			EMSG("Failed to set Tree-View style from settings")));
		return false;
	}

	return true;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Sets the dirtiness (unsaved changes) status of the document and updates the
 * interface accordingly.
 *
 * @param bDirty Does this document currently contain unsaved changes?
 */
void BolotaView::SetDirty(bool bDirty) {
	// Set document as dirty.
	if (bDirty != m_doc->IsDirty())
		m_doc->SetDirty(bDirty);

	// Check if we need to change the window's title.
	TCHAR szFlag[2];
	GetWindowText(m_hwndParent, szFlag, 2);
	if (((szFlag[0] == '*') && bDirty) || ((szFlag[0] != '*') && !bDirty))
		return;

	// Update the main window title and add asterisk.
	LPTSTR szTitle = GetWindowTextAlloc(m_hwndParent);
	if (!bDirty) {
		// No longer dirty.
		SetWindowText(m_hwndParent, szTitle + 1);
	} else {
		// Dirty bastard!
		LPTSTR szNewTitle = (LPTSTR)malloc((_tcslen(szTitle) + 2) *
			sizeof(TCHAR));
		szNewTitle[0] = _T('*');
		_tcscpy(szNewTitle + 1, szTitle);
		SetWindowText(m_hwndParent, szNewTitle);
		free(szNewTitle);
		szNewTitle = NULL;
	}

	// Free up our resources.
	free(szTitle);
	szTitle = NULL;
}

/**
 * Checks if the document contains unsaved changes.
 *
 * @return Does this document currently contain unsaved changes?
 */
bool BolotaView::IsDirty() const {
	if (m_doc == NULL)
		return false;

	return m_doc->IsDirty();
}

/**
 * Gets the associated field object from a Tree-View item.
 *
 * @param hti Tree-View item handle.
 *
 * @return Field object associated with the Tree-View item or NULL if an error
 *         happened.
 */
Field* BolotaView::GetFieldFromTreeItem(HTREEITEM hti) const {
	// Check if we have a valid item handle.
	if (hti == NULL) {
		ThrowError(EMSG("Tree-View item for field retrieval is NULL"));
		return NULL;
	}

	// Get the selected item from the handle.
	TVITEM tvi;
	tvi.hItem = hti;
	tvi.mask = TVIF_PARAM;
	if (!TreeView_GetItem(m_hWnd, &tvi)) {
		ThrowError(EMSG("Failed to get Tree-View item from handle"));
		return NULL;
	}

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
