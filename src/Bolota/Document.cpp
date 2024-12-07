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
	m_bDirty = false;
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
 * Sets the first topic of the topics linked list.
 *
 * @param topic New first topic of the topic linked list.
 */
void Document::SetFirstTopic(Field *topic) {
	m_topics = topic;
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
		
		SetFirstTopic(field);
		return;
	}

	// Shuffle things around.
	field->SetParent(prev->Parent(), true);
	field->SetNext(prev->Next(), false);
	prev->SetNext(field, false);

	// Flag unsaved changes.
	SetDirty(true);
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
		SetFirstTopic(field);

	// Shuffle things around.
	field->SetParent(next->Parent(), true);
	field->SetPrevious(next->Previous(), false);
	next->SetPrevious(field, false);

	// Set ourselves as the parent's child if we prepended to the first item.
	if (!field->HasPrevious() && next->HasParent())
		next->Parent()->SetChild(field, false);

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Appends a new topic to the end of the document's topic list.
 *
 * @param field Topic field to be appended to the document.
 */
void Document::AppendTopic(Field *field) {
	// Check if this is the first topic to be added to the document.
	if (m_topics == NULL) {
		SetFirstTopic(field);
		return;
	}

	// Find the last element and assign the next item to it.
	Field *last = m_topics;
	while (last->HasNext())
		last = last->Next();
	AppendTopic(last, field);

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Deletes a topic and all of its child fields from the document's topic list.
 *
 * @param field Topic field to be deleted from the document.
 */
void Document::DeleteTopic(Field *field) {
	// Ensure we pass along the first topic of the linked list.
	if (m_topics == field)
		SetFirstTopic(field->Next());
	
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

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Detaches a topic from the document's topic list and fills its gap. Should be
 * used for moving topics around.
 *
 * @warning The field will still be valid, but won't be referenced anywhere in
 *          the topics tree.
 *
 * @param field Field to be detached from the topics tree.
 */
void Document::PopTopic(Field *field) {
	// Fill the space that will be left behind by the detaching topic.
	if (field->IsFirstChild()) {
		// Is the first child.
		field->Parent()->SetChild(field->Next(), false);
		if (field->HasNext())
			field->Next()->SetPrevious(NULL, true);
	} else if (field->HasPrevious()) {
		// In the middle of two topics.
		field->Previous()->SetNext(field->Next(), false);
	} else if (field == FirstTopic()) {
		// Is the first topic of the document.
		SetFirstTopic(field->Next());
		if (m_topics != NULL) {
			m_topics->SetPrevious(NULL, true);
			m_topics->SetParent(NULL, true);
		}
	} else {
		throw std::exception("Unknown/unhandled condition when trying to pop "
			"topic from document");
	}

	// Blank out the moving object.
	field->SetParent(NULL, true);
	field->SetPrevious(NULL, true);
	field->SetNext(NULL, true);

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Moves a topic to become the first element of the topics list.
 *
 * @param field Field to become the new topmost element.
 */
void Document::MoveTopicToTop(Field *field) {
	// Just to be sure.
	Field *first = FirstTopic();
	if (field == first)
		return;

	// Detach topic and replace the topmost one.
	PopTopic(field);
	field->SetNext(first, false);
	SetFirstTopic(field);

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Moves a topic below another in the document's topic list.
 *
 * @param below Topic field to be below. This is the one that will be moved.
 * @param above Reference topic field for the move.
 */
void Document::MoveTopicBelow(Field *below, Field *above) {
	// Detach the moving topic.
	PopTopic(below);

	// Shuffle things around to make space for us at our new home.
	if (above->HasChild()) {
		// Below will become the first child of above.
		below->SetParent(above->Parent(), true);
		below->SetNext(above->Child(), false);
		above->SetChild(below, false);
	} else {
		below->SetParent(above->Parent(), true);
		below->SetNext(above->Next(), false);
		above->SetNext(below, false);
	}

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Indents a topic field relative to the one above it.
 */
void Document::IndentTopic(Field *field) {
	// Nothing can be done about top fields.
	if (!field->HasPrevious())
		throw std::exception("Cannot indent topic without previous field");
	
	// Shuffle things around in preparation for the move.
	Field *prev = field->Previous();
	PopTopic(field);

	// Perform the actual move.
	if (prev->HasChild()) {
		// Move field to become the new last child of its previous field.
		Field *last = prev->Child();
		while (last->HasNext())
			last = last->Next();
		field->SetParent(prev, true);
		last->SetNext(field, false);
	} else {
		// Field will become the new first child of its previous field.
		prev->SetChild(field, false);
	}

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * De-indents a topic field relative to its parent.
 */
void Document::DeindentTopic(Field *field) {
	// Nothing can be done about orphan fields.
	if (!field->HasParent())
		throw std::exception("Cannot deindent topic without parent field");
	
	// Shuffle things around in preparation for the move.
	Field *parent = field->Parent();
	PopTopic(field);

	// Perform the actual move.
	field->SetParent(parent->Parent(), true);
	field->SetNext(parent->Next(), false);
	field->SetPrevious(parent, false);

	// Flag unsaved changes.
	SetDirty(true);
}

/**
 * Checks if the document is currently empty.
 *
 * @return TRUE if the document has no topics yet.
 */
bool Document::IsEmpty() const {
	return m_topics == NULL;
}

/**
 * Checks the consistency of the relationships of a reference field against
 * its expected related fields.
 *
 * @param ref    Reference field to check.
 * @param parent Expected field parent.
 * @param child  Expected field child.
 * @param prev   Expected field previous.
 * @param next   Expected field next.
 *
 * @throws ConsistencyException if an inconsistency is found.
 */
void Document::CheckFieldConsistency(Field *ref, Field *parent, Field *child,
									 Field *prev, Field *next) {
	// Check ourselves first.
	ref->CheckConsistency();

	// Check expected relationships.
	if (ref->Parent() != parent)
		throw ConsistencyException(ref, parent, ref->Parent(), "Parent");
	if (ref->Child() != child)
		throw ConsistencyException(ref, child, ref->Child(), "Child");
	if (ref->Previous() != prev)
		throw ConsistencyException(ref, prev, ref->Previous(), "Previous");
	if (ref->Next() != next)
		throw ConsistencyException(ref, next, ref->Next(), "Next");
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

	// Close the file handle and mark as clean.
	self->CloseFile();
	self->SetDirty(false);

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

	// Close the file handle and mark as clean.
	CloseFile();
	SetDirty(false);

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

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Properties Getters and Setters                       |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the document's title property.
 *
 * @return Document's title property field.
 */
TextField* Document::Title() const {
	return this->m_title;
}

/**
 * Sets the document's title property.
 *
 * @warning This method takes ownership of the field object.
 *
 * @param title Document's new title field.
 */
void Document::SetTitle(TextField *title) {
	if (this->m_title)
		delete this->m_title;
	this->m_title = title;
	SetDirty(true);
}

/**
 * Sets the document's title property.
 *
 * @warning This method takes ownership of the string.
 *
 * @param szTitle Document's new title text.
 */
void Document::SetTitle(LPTSTR szTitle) {
	this->m_title->SetTextOwner(szTitle);
	SetDirty(true);
}

/**
 * Gets the document's subtitle property.
 *
 * @return Document's subtitle property field.
 */
TextField* Document::SubTitle() const {
	return this->m_subtitle;
}

/**
 * Sets the document's subtitle property.
 *
 * @warning This method takes ownership of the field object.
 *
 * @param subtitle Document's new subtitle field.
 */
void Document::SetSubTitle(TextField *subtitle) {
	if (this->m_subtitle)
		delete this->m_subtitle;
	this->m_subtitle = subtitle;
	SetDirty(true);
}

/**
 * Sets the document's subtitle property.
 *
 * @warning This method takes ownership of the string.
 *
 * @param szSubTitle Document's new subtitle text.
 */
void Document::SetSubTitle(LPTSTR szSubTitle) {
	this->m_subtitle->SetTextOwner(szSubTitle);
	SetDirty(true);
}

/**
 * Gets the document's creation date property.
 *
 * @return Document's creation date property field.
 */
DateField* Document::Date() const {
	return this->m_date;
}

/**
 * Sets the document's creation date property.
 *
 * @warning This method takes ownership of the field object.
 *
 * @param date New creation date field.
 */
void Document::SetDate(DateField *date) {
	if (this->m_date)
		delete this->m_date;
	this->m_date = date;
	SetDirty(true);
}

/**
 * Sets the document's creation date property.
 *
 * @param st New creation date based on a system time object.
 */
void Document::SetDate(const SYSTEMTIME *st) {
	this->m_date->SetTimestamp(st);
	SetDirty(true);
}

/**
 * Sets the dirtiness (unsaved changes) status of the document.
 *
 * @param dirty Does this document currently contain unsaved changes?
 */
void Document::SetDirty(bool dirty) {
	this->m_bDirty = dirty;
}

/**
 * Checks if a document contains unsaved changes.
 *
 * @return Does this document currently contain unsaved changes?
 */
bool Document::IsDirty() const {
	return this->m_bDirty;
}
