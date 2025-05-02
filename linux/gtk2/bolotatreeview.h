/**
 * bolotatreeview.h
 * GtkTreeView responsible for rendering a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_GTK2_BOLOTATREEVIEW_H
#define _BOLOTA_GTK2_BOLOTATREEVIEW_H

#include <gtk/gtk.h>

#include <Document.h>

class BolotaTreeView {
private:
	Bolota::Document *document;

public:
	GtkWidget *widget;

	// Constructors and destructors.
	BolotaTreeView();
	virtual ~BolotaTreeView();

	// File operations.
	void OpenDocument(Bolota::Document *doc);
	void OpenExampleDocument();

	// TreeView operations.
	void AddTreeViewItem(GtkTreeStore *store, GtkTreeIter *parent,
		GtkTreeIter *prev, Bolota::Field *field);
};

#endif // _BOLOTA_GTK2_BOLOTATREEVIEW_H
