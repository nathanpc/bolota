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
	m_hFile = NULL;
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

	// Close the file handle.
	if ((m_hFile != NULL) && (m_hFile != INVALID_HANDLE_VALUE)) {
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
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
	field->SetParent(prev->Parent(), true);
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
	field->SetParent(next->Parent(), true);

	// Set ourselves as the parent's child if we prepended to the first item.
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

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             File Operations                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Writes the document to the currently associated file.
 *
 * @return Number of bytes written to the file.
 */
size_t Document::WriteFile() {
	if (!HasFileAssociated())
		throw std::exception("No file associated to write to");
	return WriteFile(m_strPath.GetNativeString(), false);
}

/**
 * Writes the document to a file specified by its path.
 *
 * @warning Will always overwrite existing files.
 *
 * @param szPath     Path of the file to receive the contents of the object.
 * @param bAssociate Associate the file we are about to save with the document?
 *
 * @return Number of bytes written to the file.
 */
size_t Document::WriteFile(LPCTSTR szPath, bool bAssociate) {
	size_t ulBytes = 0;
	DWORD dwWritten = 0;

	// Open a file handle for us to operate on.
	m_hFile = CreateFile(szPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		throw std::exception("CreateFile failed");
	if (bAssociate)
		m_strPath = szPath;

	// Write file header.
	::WriteFile(m_hFile, BOLOTA_DOC_MAGIC, BOLOTA_DOC_MAGIC_LEN, &dwWritten,
		NULL);
	ulBytes += dwWritten;
	uint8_t iVersion = BOLOTA_DOC_VER;
	::WriteFile(m_hFile, &iVersion, sizeof(uint8_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// Write properties section length.
	uint32_t ulSectionLength = PropertiesLength();
	::WriteFile(m_hFile, &ulSectionLength, sizeof(uint32_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// Write topics section length.
	ulSectionLength = TopicsLength();
	::WriteFile(m_hFile, &ulSectionLength, sizeof(uint32_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// TODO: Write attachments section length.
	/*ulSectionLength = 0;
	::WriteFile(m_hFile, &ulSectionLength, sizeof(uint32_t), &dwWritten, NULL);
	ulBytes += dwWritten;*/

	// Write document sections.
	ulBytes += WriteProperties();
	ulBytes += WriteTopics();
	// TODO: ulBytes += WriteAttachments();

	// Close the file handle.
	CloseHandle(m_hFile);
	m_hFile = NULL;

	return ulBytes;
}

/**
 * Writes the properties section of the file.
 *
 * @return Number of bytes written to the file.
 */
size_t Document::WriteProperties() const {
	size_t ulBytes = 0;

	// Write the property fields.
	ulBytes += m_title->Write(m_hFile);
	ulBytes += m_subtitle->Write(m_hFile);
	ulBytes += m_date->Write(m_hFile);

	return ulBytes;
}

/**
 * Writes a topic field linked list to the file.
 *
 * @param field Field to be written to the file. Will include its childs and
 *              simblings.
 *
 * @return Number of bytes written to the file.
 */
size_t Document::WriteTopics(Field *field) const {
	size_t ulBytes = 0;

	// Go through the fields recursively.
	do {
		ulBytes += field->Write(m_hFile);
		if (field->HasChild())
			ulBytes += WriteTopics(field->Child());
		field = field->Next();
	} while (field != NULL);

	return ulBytes;
}

/**
 * Writes the topics section of the file.
 *
 * @return Number of bytes written to the file.
 */
size_t Document::WriteTopics() const {
	return WriteTopics(m_topics);
}

/**
 * Gets the length of the properties section of the file.
 *
 * @return Length of the properties section in bytes.
 */
uint32_t Document::PropertiesLength() const {
	return m_title->Length() + m_subtitle->Length() + m_date->Length();
}

/**
 * Gets the length of the topics section of the file.
 *
 * @return Length of the topics section in bytes.
 */
uint32_t Document::TopicsLength() const {
	return TopicsLength(m_topics);
}

/**
 * Gets the entire length of a linked list of topic fields.
 *
 * @param field Field to calculate its length and of its childs and simblings.
 *
 * @return Length of the topics in bytes.
 */
uint32_t Document::TopicsLength(Field *field) const {
	uint32_t ulLength = 0;

	// Go through the fields recursively.
	do {
		ulLength += field->Length();
		if (field->HasChild())
			ulLength += TopicsLength(field->Child());
		field = field->Next();
	} while (field != NULL);

	return ulLength;
}

/**
 * Checks if there is a file associated with this document.
 *
 * @return TRUE if this document is associated to a persistent file.
 */
bool Document::HasFileAssociated() const {
	return !m_strPath.Empty();
}

/**
 * Gets the path to the file currently associated with this document object.
 *
 * @return Path to the associated document file.
 */
UString& Document::FilePath() {
	return m_strPath;
}
