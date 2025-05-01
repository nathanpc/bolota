/**
 * mainwindow.h
 * Main application window.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_GTK2_MAINWINDOW_H
#define _BOLOTA_GTK2_MAINWINDOW_H

#include <gtk/gtk.h>

class MainWindow {
private:
	GtkWidget *window;

public:
	// Constructors and destructors.
	MainWindow();
	virtual ~MainWindow();

	// Event handlers.
	static void Event_Destroy(const GtkWidget* widget, gpointer data);
};

#endif // _BOLOTA_GTK2_MAINWINDOW_H
