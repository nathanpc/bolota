/**
 * mainwindow.cpp
 * Main application window.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "mainwindow.h"
#include "gtk/gtk.h"

#include <gdk/gdkkeysyms.h>

/**
 * Sets up the main window object and shows it.
 */
MainWindow::MainWindow() {
	// Create the window, set it up, and show it.
	this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Bolota");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	// Setup document viewer.
	this->tree_view = new BolotaTreeView(window);

	// Setup accelerator.
	this->accel = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel);

	// Setup main vertical layout box.
	vbox = gtk_vbox_new(false, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	menubar = SetupMenubar();
	toolbar = SetupToolbar();
	gtk_box_pack_start(GTK_BOX(vbox), menubar, false, false, 0);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, false, false, 0);
	gtk_box_pack_start(GTK_BOX(vbox), tree_view->widget, true, true, 0);

	// Setup window closing event.
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(MainWindow::Event_Destroy), this);

	// Show all widgets.
	gtk_widget_show_all(this->window);
	tree_view->OpenExampleDocument();
}

/**
 * Takes care of deallocating the window and closing it if needed.
 */
MainWindow::~MainWindow() {
	// Destroy the window itself.
	if (window != nullptr)
		MainWindow::Event_Destroy(nullptr, (gpointer)this);

	// Free the document viewer widget.
	if (tree_view != nullptr) {
		delete tree_view;
		tree_view = nullptr;
	}
}

/**
 * Sets up the menu for the application.
 *
 * @return Menu bar widget.
 */
GtkWidget* MainWindow::SetupMenubar() {
	GtkWidget* menu;
	GtkWidget* item;
	GtkWidget* icon;

	// Create the menu bar widget.
	GtkWidget* bar = gtk_menu_bar_new();

	// File menu.
	menu = gtk_menu_new();
	GtkWidget* file_menu = gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu), menu);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_REFRESH, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_PROPERTIES, accel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(Event_Destroy),
					 this);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_menu);

	// Field menu.
	menu = gtk_menu_new();
	GtkWidget* field_menu = gtk_menu_item_new_with_mnemonic("Fi_eld");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(field_menu), menu);
	item = gtk_image_menu_item_new_with_mnemonic("_Edit...");
	gtk_widget_add_accelerator(item, "activate", accel, GDK_i,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Delete...");
	icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_d,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Prepend Field...");
	icon = gtk_image_new_from_icon_name("go-top", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_u,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Append Field...");
	icon = gtk_image_new_from_icon_name("go-bottom", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_o,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Create Child Field...");
	icon = gtk_image_new_from_icon_name("go-jump", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_p,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("Append to _Parent Field...");
	icon = gtk_image_new_from_icon_name("go-first", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_y,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("Move _Up");
	icon = gtk_image_new_from_icon_name("up", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_k,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK(BolotaTreeView::Event_MoveUp), tree_view);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("Move Dow_n");
	icon = gtk_image_new_from_icon_name("down", GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_j,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("_Indent");
	icon = gtk_image_new_from_icon_name("format-indent-more",
										GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_l,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	item = gtk_image_menu_item_new_with_mnemonic("D_e-indent");
	icon = gtk_image_new_from_icon_name("format-indent-less",
										GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icon);
	gtk_widget_add_accelerator(item, "activate", accel, GDK_h,
							   (GdkModifierType)0, GTK_ACCEL_VISIBLE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_shell_append(GTK_MENU_SHELL(bar), field_menu);

	return bar;
}

/**
 * Sets up the toolbar for the application.
 *
 * @return Toolbar widget.
 */
GtkWidget* MainWindow::SetupToolbar() {
	GtkToolItem* item;
	GtkWidget* icon;

	// Create the toolbar.
	GtkWidget* bar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);
	GtkIconSize ics = GTK_ICON_SIZE_LARGE_TOOLBAR;

	// File operation buttons.
	item = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);

	// Document operation buttons.
	item = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);

	// Field creation buttons.
	icon = gtk_image_new_from_icon_name("go-top", ics);
	item = gtk_tool_button_new(icon, "Prepend");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	icon = gtk_image_new_from_icon_name("go-bottom", ics);
	item = gtk_tool_button_new(icon, "Append");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	icon = gtk_image_new_from_icon_name("go-jump", ics);
	item = gtk_tool_button_new(icon, "Create Child");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	icon = gtk_image_new_from_icon_name("go-first", ics);
	item = gtk_tool_button_new(icon, "Append to Parent");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);

	// Field movement buttons.
	icon = gtk_image_new_from_icon_name("up", ics);
	item = gtk_tool_button_new(icon, "Move Up");
	g_signal_connect(G_OBJECT(item), "clicked",
					 G_CALLBACK(BolotaTreeView::Event_MoveUp), tree_view);
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	icon = gtk_image_new_from_icon_name("down", ics);
	item = gtk_tool_button_new(icon, "Move Down");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);

	// Field indentation buttons.
	icon = gtk_image_new_from_icon_name("format-indent-more", ics);
	item = gtk_tool_button_new(icon, "Indent");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);
	icon = gtk_image_new_from_icon_name("format-indent-less", ics);
	item = gtk_tool_button_new(icon, "De-indent");
	gtk_toolbar_insert(GTK_TOOLBAR(bar), item, -1);

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
