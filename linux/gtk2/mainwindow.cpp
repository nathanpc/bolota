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
	menubar = SetupMenu();
	gtk_box_pack_start(GTK_BOX(vbox), menubar, false, false, 0);
	//gtk_box_pack_start(GTK_BOX(vbox), tree_view.widget, true, true, 1);

	// Setup window closing event.
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(MainWindow::Event_Destroy), this);

	// Show all widgets.
	gtk_widget_show_all(this->window);
	tree_view.OpenExampleDocument();
}

/**
 * Takes care of deallocating the window and closing it if needed.
 */
MainWindow::~MainWindow() {
	if (window != nullptr)
		MainWindow::Event_Destroy(nullptr, (gpointer)this);
}

/**
 * Sets up the menu for the application.
 *
 * @return Menu bar widget.
 */
GtkWidget* MainWindow::SetupMenu() {
	GtkWidget* menu;
	GtkWidget* item;

	// Create the menu bar widget.
	GtkWidget* bar = gtk_menu_bar_new();

	// File menu.
	menu = gtk_menu_new();
	GtkWidget* file_menu = gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu), menu);
	item = gtk_image_menu_item_new_with_mnemonic("_New");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Open");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Save");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("Save _As...");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Refresh");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("Document _Properties...");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Quit");
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(Event_Destroy),
					 this);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_menu);

	return bar;
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
