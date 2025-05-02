/**
 * bolotatreeview.cpp
 * GtkTreeView responsible for rendering a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "bolotatreeview.h"

#include <Document.h>

using namespace Bolota;

/**
 * Anonymous enum for TreeView column indexing.
 */
enum {
	COL_TEXT = 0,
	NUM_COLS
};

/**
 * Initializes the TreeView widget.
 */
BolotaTreeView::BolotaTreeView() {
	// Create TreeView widget.
	this->widget = gtk_tree_view_new();

	// Setup text column.
	GtkTreeViewColumn *col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Text");
	gtk_tree_view_append_column(GTK_TREE_VIEW(this->widget), col);

	// Setup text column cell renderer.
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, true);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COL_TEXT);
}

/**
 * Free up any resources used by the document viewer.
 */
BolotaTreeView::~BolotaTreeView() {
}
