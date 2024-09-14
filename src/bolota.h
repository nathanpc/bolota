/**
 * bolota.h
 * The parser for bolota binary files.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef BOLOTA_PARSER_H_
#define BOLOTA_PARSER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bolota parser internal error codes.
 */
typedef enum bolota_err_e {
	BOLOTA_OK = 0,
	BOLOTA_ERR_ALLOC,
	BOLOTA_UNKNOWN = 0xFF
} bec_t;

/**
 * An 8-bit value that determines the type of a field.
 */
typedef enum bolota_type_e {
	BOLOTA_TYPE_IGNORE = 0,
	BOLOTA_TYPE_TEXT = 'T',
	BOLOTA_TYPE_DATE = 'd'
} bolota_type_t;

/**
 * A line of a note in a document.
 */
typedef struct bolota_field_s {
	/* Header */
	bolota_type_t type;
	uint8_t depth;
	uint16_t length;

	/* Data */
	union {
		char *text;
		uint8_t *data;
	};

	/* Not saved to file. */
	struct bolota_field_s *prev;
	struct bolota_field_s *next;
} bolota_field_t;

/**
 * An entire bolota document.
 */
typedef struct bolota_doc_s {
	/* Header */
	uint8_t magic;
	uint8_t version;
	uint16_t props_len;
	uint32_t topics_len;
	uint32_t attach_len;

	/* Document Properties */
	struct props {
		bolota_field_t title;
		bolota_field_t subtitle;
		bolota_field_t date;
	};

	bolota_field_t *field;
} bolota_doc_t;

/* Field handling. */
bolota_field_t *bolota_field_new(bolota_type_t type, uint8_t depth);
bec_t bolota_field_set_text(bolota_field_t *field, const char *text);
void bolota_field_free(bolota_field_t *field);
void bolota_field_print(const bolota_field_t *field);

#ifdef __cplusplus
}
#endif

#endif /* BOLOTA_PARSER_H_ */