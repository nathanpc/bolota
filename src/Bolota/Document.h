/**
 * Document.h
 * Abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _BOLOTA_DOCUMENT_H
#define _BOLOTA_DOCUMENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdint.h>

#ifdef __cplusplus

#include "UString.h"
#include "Field.h"

extern "C" {
#endif // __cplusplus

/**
 * Magic string used by Bolota to identify its binary files.
 */
#define BOLOTA_DOC_MAGIC "BLT"

/**
 * Document version used by this version of the library.
 */
#define BOLOTA_DOC_VER 1

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

	/* Section: Sequence of topics fields. */
	/* Section: Sequence of attachment fields. */
} bolota_doc_t;

#ifdef __cplusplus
}

namespace Bolota {
	/**
     * Abstraction of a Bolota document.
	 */
	class Document {
	private:
		// Properties
		TextField *m_title;
		TextField *m_subtitle;
		DateField *m_date;

		// Sections
		Field *m_topics;

	public:
		// Constructors and destructors.
		Document(TextField *title, TextField *subtitle, DateField *date);
		virtual ~Document();

		// Topic management.
		Field* FirstTopic() const;
		void AppendTopic(Field *field);
	};
}

#endif // __cplusplus

#endif // _BOLOTA_DOCUMENT_H
