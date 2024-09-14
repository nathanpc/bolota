/**
 * Bolota
 * An outline note taking application.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include <string.h>

#include "bolota.h"

/**
 * Application's main entry point.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 *
 * @return Return code.
 */
int main(int argc, char **argv) {
	bolota_field_t *field;

	/* Play with the field. */
	field = bolota_field_new(BOLOTA_TYPE_TEXT, 0);
	if (field == NULL)
		return 1;
	bolota_field_set_text(field, "This is some text in a field.");
	bolota_field_print(field);

	/* Free up any resources. */
	bolota_field_free(field);
	field = NULL;

	return 0;
}