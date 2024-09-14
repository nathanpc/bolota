/**
 * bolota.c
 * The parser for bolota binary files.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "bolota.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Creates a new field object.
 *
 * @warning This function allocates memory dynamically.
 *
 * @param type  Type of the field.
 * @param depth How deep is it in the topic list?
 *
 * @return The newly allocated field object or NULL if an error occurred.
 */
bolota_field_t *bolota_field_new(bolota_type_t type, uint8_t depth) {
	bolota_field_t *field;

	/* Allocate some memory for our object. */
	field = (bolota_field_t *)malloc(sizeof(bolota_field_t));
	if (field == NULL)
		return NULL;

	/* Populate the object with some sane defaults. */
	field->type = type;
	field->depth = depth;
	field->length = 0;
	field->text = NULL;
	field->prev = NULL;
	field->next = NULL;

	return field;
}

/**
 * Sets the text of a field. Will override the current value if needed and deal
 * with any memory management that's required.
 *
 * @param field Field object to be modified.
 * @param text  New text to be associated with the field.
 *
 * @return Error code if an exception occurred.
 */
bec_t bolota_field_set_text(bolota_field_t *field, const char *text) {
	/* Calculate the size of the new field text. */
	field->length = strlen(text);

	/* Reallocate the text field if needed. */
	if (field->text) {
		char *old_text = field->text;
		field->text = (char *)realloc(field->text,
		                              (field->length + 1) * sizeof(char));

		/* Check if the reallocation failed and rollback changes. */
		if (field->text == NULL) {
			field->length = strlen(old_text);
			field->text = old_text;
			return BOLOTA_ERR_ALLOC;
		}
	} else {
		field->text = (char *)malloc((field->length + 1) * sizeof(char));
		if (field->text == NULL) {
			field->length = 0;
			return BOLOTA_ERR_ALLOC;
		}
	}

	/* Copy the new string over and return. */
	strcpy(field->text, text);
	return BOLOTA_OK;
}

/**
 * Frees up any memory allocated for a field object and invalidates any
 * references to it in the linked list.
 *
 * @param field Field object to be freed.
 */
void bolota_field_free(bolota_field_t *field) {
	/* Invalidate ourselves and free internal allocations. */
	field->type = BOLOTA_TYPE_IGNORE;
	if (field->data) {
		free(field->data);
		field->data = NULL;
	}

	/* Invalidate references to this object in the linked list. */
	if (field->next) {
		field->next->prev = NULL;
		field->next = NULL;
	}
	if (field->prev) {
		field->prev->next = NULL;
		field->prev = NULL;
	}

	/* Free ourselves. */
	free(field);
	field = NULL;
}

/**
 * Prints out the contents of a field. To be used for debugging purposes.
 *
 * @param field Field object to be printed out.
 */
void bolota_field_print(const bolota_field_t *field) {
	printf("[%u] (%c) %s [%u]\n", field->depth, field->type, field->text,
	       field->length);
}
