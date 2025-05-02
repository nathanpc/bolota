/**
 * mainwindow.cpp
 * Main application window.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "mainwindow.h"
#include "gtk/gtk.h"

/**
 * Sets up the main window object and shows it.
 */
MainWindow::MainWindow() {
	// Create the window, set it up, and show it.
	this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Bolota");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	// Setup main vertical layout box.
	vbox = gtk_vbox_new(false, 2);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	// Add document viewer TreeView.
	gtk_box_pack_start(GTK_BOX(vbox), tree_view.widget, true, true, 1);

	// Setup window closing event.
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(MainWindow::Event_Destroy), this);

	// Show all widgets.
	gtk_widget_show_all(this->window);
}

/**
 * Takes care of deallocating the window and closing it if needed.
 */
MainWindow::~MainWindow() {
	if (window != nullptr)
		MainWindow::Event_Destroy(nullptr, (gpointer)this);
}

/**
 * GTK "destroy" event handler.
 *
 * @param widget Widget that called the event.
 * @param data   Pointer to the window object.
 */
void MainWindow::Event_Destroy(const GtkWidget* widget, gpointer data) {
	MainWindow *pThis = static_cast<MainWindow*>(data);
	gtk_widget_destroy(pThis->window);
	pThis->window = nullptr;
	gtk_main_quit();
}
