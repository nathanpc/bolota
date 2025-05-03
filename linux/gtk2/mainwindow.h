/**
 * mainwindow.h
 * Main application window.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_GTK2_MAINWINDOW_H
#define _BOLOTA_GTK2_MAINWINDOW_H

#include <gtk/gtk.h>

#include "bolotatreeview.h"

class MainWindow {
private:
	// GTK stuff.
	GtkWidget *window;
	GtkAccelGroup *accel;
	GtkWidget *vbox;
	GtkWidget *menubar;
	GtkWidget *toolbar;

	// Custom controls.
	BolotaTreeView *tree_view;

   public:
	// Constructors and destructors.
	MainWindow();
	virtual ~MainWindow();

	// Setup and things.
	GtkWidget* SetupMenubar();
	GtkWidget* SetupToolbar();

	// Event handlers.
	static void Event_Destroy(const GtkWidget* widget, gpointer data);
};

#endif // _BOLOTA_GTK2_MAINWINDOW_H
