/**
 * Document.cpp
 * Abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Document.h"

#include "Exceptions.h"

using namespace Bolota;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Constrcuts an empty Bolota document object.
 */
Document::Document() {
	Initialize(NULL, NULL, NULL, NULL, NULL);
}

/**
 * Construct a Bolota document object.
 *
 * @param title    Document title.
 * @param subtitle Document subtitle with more context.
 * @param date     Date and time when the note was taken.
 */
Document::Document(TextField *title, TextField *subtitle, DateField *date) {
	Initialize(title, subtitle, date, NULL, NULL);
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

/**
 * A simple constructor helper for common things we need to initialize.
 *
 * @param title    Document title.
 * @param subtitle Document subtitle with more context.
 * @param date     Date and time when the note was taken.
 * @param szPath   A path to a file to be associated with this object.
 * @param hFile    A file handle to be associated with the object.
 */
void Document::Initialize(TextField *title, TextField *subtitle,
						  DateField *date, LPCTSTR szPath, HANDLE hFile) {
	m_title = title;
	m_subtitle = subtitle;
	m_date = date;
	m_topics = NULL;
	m_hFile = hFile;
	if (szPath != NULL)
		m_strPath = szPath;
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
	// Check if this is the first topic to be added to the document.
	if (m_topics == NULL) {
		if (prev != NULL) {
			throw std::exception("Tried to append a topic from the middle of "
				"the linked list as the root element");
		}
		
		m_topics = field;
		return;
	}

	// Shuffle things around.
	field->SetParent(prev->Parent(), true);
	field->SetNext(prev->Next(), false);
	prev->SetNext(field, false);
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
	field->SetParent(next->Parent(), true);
	field->SetPrevious(next->Previous(), false);
	next->SetPrevious(field, false);

	// Set ourselves as the parent's child if we prepended to the first item.
	if (!field->HasPrevious() && next->HasParent())
		next->Parent()->SetChild(field, false);
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

/**
 * Deletes a topic and all of its child fields from the document's topic list.
 *
 * @param field Topic field to be deleted from the document.
 */
void Document::DeleteTopic(Field *field) {
	// Ensure we pass along the first topic of the linked list.
	if (m_topics == field)
		m_topics = field->Next();
	
	// Rearrange parent's first child if needed.
	if (field->IsFirstChild())
		field->Parent()->SetChild(field->Next(), false);

	// Shuffle things around.
	if (field->HasPrevious()) {
		field->Previous()->SetNext(field->Next(), false);
	} else if (field->HasNext()) {
		field->Next()->SetPrevious(NULL, true);
	}

	// Delete the field and all of its children.
	field->Destroy(true, false);
}

/**
 * Moves a topic above another in the document's topic list.
 *
 * @param above Topic field to be above. This is the one that will be moved.
 * @param below Reference topic field for the move.
 */
void Document::MoveTopicAbove(Field *above, Field *below) {
	// Are we moving to the first node in our document?
	if (below == m_topics)
		m_topics = above;

	// Fill the space left behind by the moved topic.
	if (above->IsFirstChild()) {
		above->Parent()->SetChild(above->Next(), false);
		if (above->HasNext())
			above->Next()->SetPrevious(NULL, true);
	} else if (above->HasPrevious()) {
		above->Previous()->SetNext(above->Next(), false);
	}

	// Shuffle things around to make space for us at our new home.
	if (below->IsFirstChild()) {
		below->Parent()->SetChild(above, false);
	} else {
		above->SetParent(below->Parent(), true);
	}
	above->SetPrevious(below->Previous(), false);
	above->SetNext(below, false);
}

/**
 * Moves a topic below another in the document's topic list.
 *
 * @param below Topic field to be below. This is the one that will be moved.
 * @param above Reference topic field for the move.
 */
void Document::MoveTopicBelow(Field *below, Field *above) {
	// Fill the space left behind by the moved topic.
	if (below->IsFirstChild()) {
		below->Parent()->SetChild(below->Next(), false);
		if (below->HasNext())
			below->Next()->SetPrevious(NULL, true);
	} else if (below->HasPrevious()) {
		below->Previous()->SetNext(below->Next(), false);
	}

	// Shuffle things around to make space for us at our new home.
	if (above->HasChild()) {
		below->SetNext(above->Child(), false);
		above->SetPrevious(below->Previous(), false);
		above->SetChild(below, false);
		below->SetPrevious(NULL, true);
	} else {
		below->SetNext(above->Next(), false);
		below->SetPrevious(above, false);
	}
}

/**
 * Checks if the document is currently empty.
 *
 * @return TRUE if the document has no topics yet.
 */
bool Document::IsEmpty() const {
	return m_topics == NULL;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             File Operations                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Reads a document object from a file.
 *
 * @param szPath Path to the file to be read and parsed into an object.
 *
 * @return The object representation of the read document.
 */
Document* Document::ReadFile(LPCTSTR szPath) {
	size_t ulLength = 0;
	DWORD dwRead = 0;

	// Open a file handle for us to operate on.
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		throw SystemException("Could not open file for reading");

	// Read the file magic anc check if it's a Bolota document.
	char szMagic[BOLOTA_DOC_MAGIC_LEN + 1];
	if (!::ReadFile(hFile, szMagic, BOLOTA_DOC_MAGIC_LEN, &dwRead, NULL))
		throw ReadError(hFile, ulLength, true);
	ulLength += dwRead;
	szMagic[BOLOTA_DOC_MAGIC_LEN] = '\0';
	if (strcmp(szMagic, BOLOTA_DOC_MAGIC))
		throw InvalidMagic(hFile);

	// Read and check if the version number is compatible.
	uint8_t ucVersion = 0;
	if (!::ReadFile(hFile, &ucVersion, sizeof(uint8_t), &dwRead, NULL))
		throw ReadError(hFile, ulLength, true);
	ulLength += dwRead;
	if (ucVersion < BOLOTA_DOC_VER)
		throw InvalidVersion(hFile);

	//  Read the length of the properties section.
	uint32_t dwLengthProp = 0;
	if (!::ReadFile(hFile, &dwLengthProp, sizeof(uint32_t), &dwRead, NULL))
		throw ReadError(hFile, ulLength, true);
	ulLength += dwRead;

	// Read the length of the topics section.
	uint32_t dwLengthTopics = 0;
	if (!::ReadFile(hFile, &dwLengthTopics, sizeof(uint32_t), &dwRead, NULL))
		throw ReadError(hFile, ulLength, true);
	ulLength += dwRead;

	// Create the new document and start parsing.
	Document *self = new Document();
	self->m_hFile = hFile;
	self->m_strPath = szPath;
	self->ReadProperties(&ulLength);
	self->ReadTopics(dwLengthTopics, &ulLength);

	// Close the file handle.
	self->CloseFile();

	return self;
}

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
		throw SystemException("Could not open file for writing");
	if (bAssociate)
		m_strPath = szPath;

	// Write file header.
	::WriteFile(m_hFile, BOLOTA_DOC_MAGIC, BOLOTA_DOC_MAGIC_LEN, &dwWritten,
		NULL);
	ulBytes += dwWritten;
	uint8_t ucVersion = BOLOTA_DOC_VER;
	::WriteFile(m_hFile, &ucVersion, sizeof(uint8_t), &dwWritten, NULL);
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
	CloseFile();

	return ulBytes;
}

/**
 * Reads the property fields from a file parsing them into the object.
 *
 * @param ulBytes Pointer to the counter storing the number of bytes read from
 *                the file so far.
 */
void Document::ReadProperties(size_t *ulBytes) {
	uint8_t ucDepth = 0;

	// Read each property field in order.
	m_title = static_cast<TextField*>(Field::Read(m_hFile, ulBytes, &ucDepth));
	m_subtitle = static_cast<TextField*>(Field::Read(m_hFile, ulBytes,
		&ucDepth));
	m_date = static_cast<DateField*>(Field::Read(m_hFile, ulBytes, &ucDepth));
}

/**
 * Reads the topics section of the file into the topics linked list.
 *
 * @param dwLengthTopics Length of the topics scetion of the file.
 * @param ulBytes        Pointer to the counter storing the number of bytes read
 *                       from the file so far.
 */
void Document::ReadTopics(uint32_t dwLengthTopics, size_t *ulBytes) {
	size_t ulStartBytes = *ulBytes;
	uint8_t ucLastDepth = 0;
	uint8_t ucDepth = 0;
	
	// Go through the fields section parsing out individual fields.
	Field *fieldLast = NULL;
	Field *field = NULL;
	while ((*ulBytes - ulStartBytes) < dwLengthTopics) {
		// Read the field.
		field = Field::Read(m_hFile, ulBytes, &ucDepth);

		if (ucDepth > ucLastDepth) {
			// Child of the last field.
			if ((ucDepth - ucLastDepth) > 1)
				throw std::exception("Field depth forward jump greater than 1");
			field->SetParent(fieldLast, false);
		} else if (ucDepth < ucLastDepth) {
			// Next topic of the parent field.
			Field *parent = fieldLast->Parent();
			while (parent->Depth() != ucDepth)
				parent = parent->Parent();
			parent->SetNext(field, false);
		} else {
			// This is just the next field in line.
			AppendTopic(fieldLast, field);
		}

		// Set the last field for the next iteration.
		ucLastDepth = ucDepth;
		fieldLast = field;
	}
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
	return m_title->FieldLength() + m_subtitle->FieldLength() +
		m_date->FieldLength();
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
		ulLength += field->FieldLength();
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

/**
 * Closes the file handle associated with this document.
 */
void Document::CloseFile() {
	CloseHandle(m_hFile);
	m_hFile = NULL;
}
