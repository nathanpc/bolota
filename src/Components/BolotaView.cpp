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
 * Adds a topic field item to the Tree-View.
 *
 * @param htiParent      Parent Tree-View item.
 * @param htiInsertAfter Tree-View item that will appear before this one.
 * @param field          Field to be added to the view.
 *
 * @return Tree-View item that was just added.
 */
HTREEITEM BolotaView::AddTreeViewItem(HTREEITEM htiParent,
									  HTREEITEM htiInsertAfter, Field *field) {
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
	
	// Go through child and next fields inserting them into the Tree-View as well.
	if (field->HasChild())
		AddTreeViewItem(hti, TVI_FIRST, field->Child());

	// Insert next fields into the Tree-View as well.
	if (field->HasNext())
		AddTreeViewItem(htiParent, hti, field->Next());

	return hti;
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
	AddTreeViewItem(TVI_ROOT, TVI_FIRST, m_doc->FirstTopic());
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
 * Gets the component's window handle.
 *
 * @return Associated window handle.
 */
HWND BolotaView::WindowHandle() const {
	return m_hWnd;
}
