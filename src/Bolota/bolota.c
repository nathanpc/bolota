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

/*
 * +===========================================================================+
 * |                                                                           |
 * |                            Document Handling                              |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Creates a new empty document object.
 *
 * @warning This function allocates memory dynamically.
 *
 * @return The newly allocated document object or NULL if an error occurred.
 */
bolota_doc_t *bolota_doc_new(void) {
	bolota_doc_t *doc;

	/* Allocate memory for document object. */
	doc = (bolota_doc_t *)malloc(sizeof(bolota_doc_t));
	if (doc == NULL)
		return NULL;

	/* Set some sane defaults. */
	doc->magic[0] = 'B';
	doc->magic[1] = 'L';
	doc->magic[2] = 'T';
	doc->version = 1;
	doc->length.props = 0;
	doc->length.topics = 0;
	doc->length.attach = 0;
	doc->props.title = bolota_field_new(BOLOTA_TYPE_TEXT, 0);
	doc->props.subtitle = bolota_field_new(BOLOTA_TYPE_TEXT, 0);
	doc->props.date = bolota_field_new(BOLOTA_TYPE_DATE, 0);
	doc->topics = NULL;
	doc->attachments = NULL;

	return doc;
}

/**
 * Sets the contents of a property in a document.
 *
 * @param doc  Document object.
 * @param prop Property field that should be edited.
 * @param text New text to assign to the property field.
 *
 * @return Error code if an exception occurred.
 */
bec_t bolota_doc_prop_set_str(bolota_doc_t *doc, bolota_field_t *prop,
                              const char *text) {
	bec_t err;

	/* Set the text in the property field. */
	err = bolota_field_set_text(prop, text);
	if (err > BOLOTA_OK)
		return err;

	/* Recalculate the property section length. */
	doc->length.props = doc->props.title->length + doc->props.subtitle->length +
	                    doc->props.date->length;

	return BOLOTA_OK;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                              Field Handling                               |
 * |                                                                           |
 * +===========================================================================+
 */

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
