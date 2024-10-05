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
	bolota_type_t type;  /* Field type. (uint8) */
	uint8_t depth;       /* Level of indentation of the topic in the note. */
	uint16_t length;     /* Length of the data part of the field (does not
                          * include header). */

	/* Data */
	union {
		char *text;      /* Text associated with the field. (not NUL terminated */
		uint8_t *data;   /* when saved to file) */
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
	char magic[3];        /* File magic definition. Should be 'BLT'. */
	uint8_t version;      /* Version number of the file specification. */
	struct {
		uint16_t props;   /* Length of the entire properties section. */
		uint32_t topics;  /* Length of the entire topics section. */
		uint32_t attach;  /* Length of the entire attachments section. */
	} length;             /* All lengths in this section are in bytes. */

	/* Document Properties */
	struct {
		bolota_field_t *title;     /* Title text for the document. */
		bolota_field_t *subtitle;  /* Subtitle text of the document. */
		bolota_field_t *date;      /* Date when the note was created. */
	} props;                       /* Various properties about the document. */

	/* Not saved to file as is. Saved in sequence and not as a linked list. */
	bolota_field_t *topics;       /* Linked list of topics fields. */
	bolota_field_t *attachments;  /* Linked list of topics fields. */
} bolota_doc_t;

/* Document handling. */
bolota_doc_t *bolota_doc_new(void);
bec_t bolota_doc_prop_set_str(bolota_doc_t *doc, bolota_field_t *prop,
                              const char *text);

/* Field handling. */
bolota_field_t *bolota_field_new(bolota_type_t type, uint8_t depth);
bec_t bolota_field_set_text(bolota_field_t *field, const char *text);
void bolota_field_free(bolota_field_t *field);
void bolota_field_print(const bolota_field_t *field);

#ifdef __cplusplus
}
#endif

#endif /* BOLOTA_PARSER_H_ */
