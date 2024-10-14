/**
 * Document.cpp
 * Abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Document.h"

using namespace Bolota;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Construct a Bolota document object.
 *
 * @param title    Document title.
 * @param subtitle Document subtitle with more context.
 * @param date     Date and time when the note was taken.
 */
Document::Document(TextField *title, TextField *subtitle, DateField *date) {
	m_title = title;
	m_subtitle = subtitle;
	m_date = date;
	m_topics = NULL;
}

/**
 * Handles a clean destruction of the document object and all if its internal
 * structures.
 */
Document::~Document() {
	// Destroy document properties.
	delete m_title;
	delete m_subtitle;
	delete m_date;

	// Destroy the topics linked list.
	if (m_topics)
		m_topics->Destroy(true, true);
	m_topics = NULL;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                            Topic Management                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the first topic of the topics linked list.
 *
 * @return First topic of the topic linked list or NULL if no topics are
 *         currently part of this document.
 */
Field* Document::FirstTopic() const {
	return m_topics;
}

/**
 * Appends a new topic after a field in the topics liked list.
 *
 * @param prev  Previous topic field to be modified.
 * @param field Topic field to be appended to the list.
 */
void Document::AppendTopic(Field *prev, Field *field) {
	// Shuffle things around.
	field->SetNext(prev->Next());
	prev->SetNext(field);
}

/**
 * Prepends a new topic before a field in the topics liked list.
 *
 * @param next  Next topic field (after the one to be prepended) to be modified.
 * @param field Topic field to be prepended to the list.
 */
void Document::PrependTopic(Field *next, Field *field) {
	// Are we prepending to the first element of the linked list?
	if (m_topics == next)
		m_topics = field;

	// Shuffle things around.
	field->SetPrevious(next->Previous());
	next->SetPrevious(field);

	// Ensure we inherit the parent if we prepended to the first item.
	if (!field->HasPrevious() && next->HasParent())
		next->Parent()->SetChild(field);
}

/**
 * Appends a new topic to the end of the document's topic list.
 *
 * @param field Topic field to be appended to the document.
 */
void Document::AppendTopic(Field *field) {
	// Check if this is the first topic to be added to the document.
	if (m_topics == NULL) {
		m_topics = field;
		return;
	}

	// Find the last element and assign the next item to it.
	Field *last = m_topics;
	while (last->HasNext())
		last = last->Next();
	AppendTopic(last, field);
}
