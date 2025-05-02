/**
 * bolotatreeview.cpp
 * GtkTreeView responsible for rendering a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "bolotatreeview.h"

#include <IconField.h>

using namespace Bolota;

/**
 * Anonymous enum for TreeView column indexing.
 */
enum {
	COL_TEXT = 0,
	COL_FIELD,
	NUM_COLS
};

/**
    * Initializes the TreeView widget.
 */
BolotaTreeView::BolotaTreeView() {
	// Create TreeView widget.
	this->document = nullptr;
	this->widget = gtk_tree_view_new();
	GtkTreeView *tree_view = GTK_TREE_VIEW(this->widget);

	// Setup text column.
	GtkTreeViewColumn *col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Text");
	gtk_tree_view_append_column(tree_view, col);

	// Setup text column cell renderer.
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, true);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COL_TEXT);

	// Setup view model.
	GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING,
		G_TYPE_POINTER);
	gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(store));
	g_object_unref(store);
}

/**
 * Free up any resources used by the document viewer.
 */
BolotaTreeView::~BolotaTreeView() {
	// Free up our document.
	if (document != nullptr) {
		delete document;
		document = nullptr;
	}
}

/**
 * Associates a Bolota document to the widget and populates it.
 *
 * @param doc Bolota document to be associated with this widget.
 */
void BolotaTreeView::OpenDocument(Document *doc) {
	// Associate document with widget.
	this->document = doc;

	// Get TreeView model and clear it.
	GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(
		GTK_TREE_VIEW(widget)));
	gtk_tree_store_clear(store);

	// Populate TreeView model store.
	GtkTreeIter iter;
	AddTreeViewItem(store, nullptr, &iter, doc->FirstTopic());

	// Expand all nodes.
	gtk_tree_view_expand_all(GTK_TREE_VIEW(widget));
}

/**
 * Adds a field to the TreeView store.
 *
 * @param store  TreeView model store.
 * @param parent Parent item.
 * @param prev   Previous item (append after this).
 * @param field  Bolota field object to be appended.
 */
void BolotaTreeView::AddTreeViewItem(GtkTreeStore *store, GtkTreeIter *parent,
									 GtkTreeIter *prev, Field *field) {
	while (field != nullptr) {
		// Append field to TreeView.
		gtk_tree_store_append(store, prev, parent);
		gtk_tree_store_set(store, prev,
			COL_TEXT, field->HasText() ? field->Text()->GetNativeString() : "",
			COL_FIELD, field, -1);

		// Append child if field has any.
		if (field->HasChild()) {
			GtkTreeIter child_iter;
			AddTreeViewItem(store, prev, &child_iter, field->Child());
		}

		// Go to the next field.
		field = field->Next();
	}
}

/**
 * Opens an example document to test the TreeView.
 */
void BolotaTreeView::OpenExampleDocument() {
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

	// Populate the widget.
	OpenDocument(doc);
}
