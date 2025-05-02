/**
 * bolotatreeview.h
 * GtkTreeView responsible for rendering a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_GTK2_BOLOTATREEVIEW_H
#define _BOLOTA_GTK2_BOLOTATREEVIEW_H

#include <gtk/gtk.h>

class BolotaTreeView {
public:
	GtkWidget *widget;

	// Constructors and destructors.
	BolotaTreeView();
	virtual ~BolotaTreeView();
};

#endif // _BOLOTA_GTK2_BOLOTATREEVIEW_H
