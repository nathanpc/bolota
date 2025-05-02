/**
 * gBolota
 * GTK+ port of the Bolota note taking application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "mainwindow.h"

/**
 * Application's main entry point
 *
 * @param argc Number of command-line arguments passed to the application.
 * @param argv Command-line arguments.
 *
 * @return Application's return code.
 */
int main(int argc, char **argv) {
	// Initialize GTK+, create main window and run GTK+ main loop.
	gtk_init(&argc, &argv);
	MainWindow window;
	gtk_main();

	return 0;
}
