/**
 * BolotaView.cpp
 * A TreeView component that displays and handles a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "BolotaView.h"

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
	ReloadView();
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
		new DateField());

	// Populate example document.
	doc->AppendTopic(new TextField(_T("First top element")));
	TextField *tmpField = new TextField(_T("Second top element"));
	TextField *tmpField2 = new TextField(tmpField,
		_T("A sub-element of the second element"));
	tmpField2->SetNext(new TextField(_T("Yet another sub-element of the second element")));
	doc->AppendTopic(tmpField);
	tmpField = new TextField(_T("Third top element"));
	doc->AppendTopic(tmpField);
	tmpField2 = new TextField(tmpField, _T("A sub-element of the third element"));
	tmpField2->SetChild(new TextField(_T("A sub-sub-element of the third element")));
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
 * @param bRecurse       Should we add fields recursively (childs and next)?
 *
 * @return Tree-View item root node that was just added.
 */
HTREEITEM BolotaView::AddTreeViewItem(HTREEITEM htiParent,
									  HTREEITEM htiInsertAfter, Field *field,
									  bool bRecurse) {
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

	// Return the item immediately if we are to do a single append.
	if (!bRecurse)
		return hti;
	
	// Go through child and next fields inserting them into the Tree-View as well.
	if (field->HasChild()) {
		AddTreeViewItem(hti, TVI_FIRST, field->Child(), bRecurse);
		TreeView_Expand(m_hWnd, hti, TVE_EXPAND);
	}

	// Insert next fields into the Tree-View as well.
	if (field->HasNext())
		AddTreeViewItem(htiParent, hti, field->Next(), bRecurse);

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
	return AddTreeViewItem(htiParent, htiInsertAfter, field, false);
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

	// Select the refreshed node.
	SelectTreeViewItem(hti);
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
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                              Event Handling                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Completely reloads the Tree-View component with data from the opened
 * document.
 */
void BolotaView::ReloadView() {
	// Clear the Tree-View for good measure.
	TreeView_DeleteAllItems(m_hWnd);

	// Populate the Tree-View with topics.
	AddTreeViewItem(TVI_ROOT, TVI_FIRST, m_doc->FirstTopic(), true);
}

/**
 * Resizes the component window.
 *
 * @param rc Desired position and size of the component.
 */
void BolotaView::Resize(RECT rc) const {
	SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right, rc.bottom,
		SWP_NOZORDER);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the currently selected item in the viewer.
 *
 * @param htiSelected Returns selected Tree-View item handle. Set to NULL if it
 *                    should be ignored.
 *
 * @return Currently selected field or NULL if none are selected.
 */
Field* BolotaView::GetSelectedField(HTREEITEM *htiSelected) {
	// Get the currently selected item handle in the Tree-View.
	HTREEITEM hti = TreeView_GetSelection(m_hWnd);
	if (hti == NULL)
		return NULL;

	// Get the selected item from the handle.
	TVITEM tvi;
	tvi.hItem = hti;
	tvi.mask = TVIF_PARAM;
	if (!TreeView_GetItem(m_hWnd, &tvi))
		throw std::exception("Failed to get Tree-View item from selection");

	// Return item handle.
	if (htiSelected != NULL)
		*htiSelected = hti;

	return reinterpret_cast<Field*>(tvi.lParam);
}

/**
 * Gets the currently selected item in the viewer.
 *
 * @return Currently selected field or NULL if none are selected.
 */
Field* BolotaView::GetSelectedField() {
	return GetSelectedField(NULL);
}

/**
 * Gets the component's window handle.
 *
 * @return Associated window handle.
 */
HWND BolotaView::WindowHandle() const {
	return m_hWnd;
}
