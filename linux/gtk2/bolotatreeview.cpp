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
 *
 * @param parent Parent window.
 */
BolotaTreeView::BolotaTreeView(GtkWidget *parent) {
	this->parent_window = parent;
	this->document = nullptr;

	// Create TreeView widget.
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

/**
 * Extracts the entire field object (including its children) from the TreeView.
 *
 * @param model     TreeView model object.
 * @param iter      Pointer to store the iterator to extract the field from.
 *                  Caller owned.
 * @param extensive Also include next elements in field object?
 *
 * @return Entire field object (including its children). Instance owned.
 */
Field* BolotaTreeView::ExtractFieldFromTree(GtkTreeModel *model,
											GtkTreeIter *iter, bool extensive) {
	// Get initial field object and clean it for good measure.
	Field *field = nullptr;
	gtk_tree_model_get(model, iter, COL_FIELD, &field, -1);
	field->SetParent(nullptr, true);
	field->SetChild(nullptr, true);
	field->SetPrevious(nullptr, true);
	field->SetNext(nullptr, true);

	// Check if we have child objects to get.
	GtkTreeIter child_iter;
	if (gtk_tree_model_iter_children(model, &child_iter, iter))
		field->SetChild(ExtractFieldFromTree(model, &child_iter, true));

	// Get next items in iterator.
	if (extensive && gtk_tree_model_iter_next(model, iter))
		field->SetNext(ExtractFieldFromTree(model, iter, true));

	return field;
}

/**
 * Gets the iterator and path from the current selection in the TreeView.
 *
 * @param model       Pointer to store the tree model.
 * @param iter        Pointer to store the iterator. Caller owned.
 * @param path        Pointer to store the path. Must be freed with
 *                    gtk_tree_path_free.
 * @param show_dialog Should the method automatically show an error dialog in
 *                    case of failure?
 *
 * @return TRUE if we could get the selection and populated the arguments with
 *         data, FALSE otherwise.
 */
bool BolotaTreeView::GetSelection(GtkTreeModel **model, GtkTreeIter *iter,
								  GtkTreePath **path, bool show_dialog) {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(
		GTK_TREE_VIEW(widget));
	if (!gtk_tree_selection_get_selected(selection, model, iter)) {
		if (show_dialog) {
			GtkWidget *dialog = gtk_message_dialog_new(
				GTK_WINDOW(parent_window), GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "No field selected. To "
				"perform this operation a field must be selected.");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
		}
		return false;
	}

	// Convert selection to tree path.
	*path = gtk_tree_model_get_path(*model, iter);

	return true;
}

/**
 * Moves the currently selected field up the tree.
 *
 * @param widget  Widget responsible for firing the event.
 * @param vp_this Pointer to ourselves.
 */
void BolotaTreeView::Event_MoveUp(const GtkWidget* widget, gpointer vp_this) {
	BolotaTreeView *pThis = static_cast<BolotaTreeView *>(vp_this);

	// Get model and iterator from the current selection.
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	if (!pThis->GetSelection(&model, &iter, &path, true))
		return;

	// Get the previous item iterator.
	GtkTreeIter prev_iter;
	gtk_tree_path_prev(path);
	if (!gtk_tree_model_get_iter(model, &prev_iter, path)) {
		gchar *strpath = gtk_tree_path_to_string(path);
		GtkWidget *dialog = gtk_message_dialog_new(
			GTK_WINDOW(pThis->parent_window), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Failed to get tree iterator "
			"from path \"%s\".", strpath);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		g_free(strpath);
		gtk_tree_path_free(path);
		return;
	}
	gtk_tree_path_free(path);

	// Check if the previous item has child elements.
	gint num_childs = gtk_tree_model_iter_n_children(model, &prev_iter);
	if (num_childs > 0) {
		// Get the last child of the element to move upwards linearly.
		GtkTreeIter child_iter;
		if (gtk_tree_model_iter_nth_child(model, &child_iter, &prev_iter,
				num_childs - 1)) {
			// Move item to after the last child of previous element.
			GtkTreeIter *move_iter = gtk_tree_iter_copy(&iter);
			Field *field = pThis->ExtractFieldFromTree(model, move_iter, false);
			gtk_tree_iter_free(move_iter);

			// Remove selected item and recreate it as a child of previous.
			gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
			GtkTreeIter *append_iter = gtk_tree_iter_copy(&prev_iter);
			pThis->AddTreeViewItem(GTK_TREE_STORE(model), &prev_iter,
				&child_iter, field);

			// Select newly created item and expand it.
			gtk_tree_model_iter_nth_child(model, append_iter, &prev_iter,
				num_childs);
			GtkTreePath *append_path = gtk_tree_model_get_path(model,
				append_iter);
			gtk_tree_iter_free(append_iter);
			gtk_tree_view_expand_row(GTK_TREE_VIEW(pThis->widget), append_path,
				true);
			gtk_tree_selection_select_path(gtk_tree_view_get_selection(
				GTK_TREE_VIEW(pThis->widget)), append_path);
			gtk_tree_path_free(append_path);

			return;
		} else {
			printf(EMSG("ERROR: Failed to get previous field's child elements.")
				"\n");
		}
	}

	// Actually move the item.
	gtk_tree_store_move_before(GTK_TREE_STORE(model), &iter, &prev_iter);
}
