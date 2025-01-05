/**
 * Field.cpp
 * Field object abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Field.h"

#include "Errors/ErrorCollection.h"
#include "Errors/ConsistencyError.h"
#include "DateField.h"
#include "IconField.h"

using namespace Bolota;

/*
 * +===========================================================================+
 * |                                                                           |
 * |                      Constructors and destructors                         |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Constructs a field object with a predefined type.
 *
 * @param type Type of the field.
 */
Field::Field(bolota_type_t type) {
	Initialize(type, NULL, NULL, NULL, NULL, NULL);
}

/**
 * Constructs a field object with a predefined type and text.
 *
 * @param type  Type of the field.
 * @param mbstr Text associated with the field.
 */
Field::Field(bolota_type_t type, const char *mbstr) {
	Initialize(type, new UString(mbstr), NULL, NULL, NULL, NULL);
}

/**
 * Constructs a field object with a predefined type and text.
 *
 * @param type Type of the field.
 * @param wstr Text associated with the field.
 */
Field::Field(bolota_type_t type, const wchar_t *wstr) {
	Initialize(type, new UString(wstr), NULL, NULL, NULL, NULL);
}

/**
 * Constructs a field object with a predefined type and a parent field.
 *
 * @param parent Parent field.
 * @param type   Type of the field.
 */
Field::Field(Field *parent, bolota_type_t type) {
	Initialize(type, NULL, parent, NULL, NULL, NULL);
}

/**
 * Constructs a field object with a predefined type, text, and a parent field.
 *
 * @param parent Parent field.
 * @param type   Type of the field.
 * @param mbstr  Text associated with the field.
 */
Field::Field(Field *parent, bolota_type_t type, const char *mbstr) {
	Initialize(type, new UString(mbstr), parent, NULL, NULL, NULL);
}

/**
 * Constructs a field object with a predefined type, text, and a parent field.
 *
 * @param parent Parent field.
 * @param type   Type of the field.
 * @param wstr   Text associated with the field.
 */
Field::Field(Field *parent, bolota_type_t type, const wchar_t *wstr) {
	Initialize(type, new UString(wstr), parent, NULL, NULL, NULL);
}

/**
 * Creates a copy of another field.
 *
 * @warning Relatives won't point to the copy in the linked list.
 *
 * @param field Field to be copied over.
 */
Field::Field(const Field *field) {
	Copy(field, false);
}

/**
 * Handles the object destruction and cleaning up of resources.
 */
Field::~Field() {
	if (HasText())
		delete m_text;
}

/**
 * Performs the common initialization tasks of the object.
 *
 * @param type   Our field type.
 * @param text   Text content of our field.
 * @param parent Parent field object.
 * @param child  Child field object.
 * @param prev   Previous field object in the linked list.
 * @param next   Next field object in the linked list.
 */
void Field::Initialize(bolota_type_t type, UString *text, Field *parent,
					   Field *child, Field *prev, Field *next) {
	// Set properties.
	SetType(type);
	m_text = text;

	// Setup the linked list.
	SetParent(parent, true);
	SetChild(child, false);
	SetPrevious(prev, false);
	SetNext(next, false);
}

/**
 * Destroy the field and all of its childs and/or next items in the list if
 * required.
 *
 * @param include_child Also destroy all of the child fields?
 * @param include_next  Also destroy all next fields in the list?
 */
void Field::Destroy(bool include_child, bool include_next) {
	// Recursively destroy all child fields.
	if (include_child && HasChild())
		Child()->Destroy(true, true);

	// Recursively destroy all next fields.
	if (include_next && HasNext())
		Next()->Destroy(true, true);

	// Commit suicide.
	delete this;
}

/**
 * Creates a duplicate of the field with the option of taking its place in the
 * linked list.
 *
 * @warning Does not copy the field type.
 *
 * @param field    Field to be copied over.
 * @param bReplace Should we change references in its relatives to point to the
 *                 new copied object?
 */
void Field::Copy(const Field *field, bool bReplace) {
	// Basics
	if (field->HasText())
		SetText(field->Text()->GetNativeString());

	// Linked list.
	SetParent(field->Parent(), !(bReplace && field->IsFirstChild()));
	SetChild(field->Child(), !bReplace);
	SetPrevious(field->Previous(), !bReplace);
	SetNext(field->Next(), !bReplace);
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             File Operations                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Reads a field from a file into a fully populated and specific field object
 * that can later be cast to the appropriate object type for its field type.
 *
 * @param hFile File handle to read the field from.
 * @param bytes Pointer to the counter storing the number of bytes read so far.
 * @param depth Pointer to store the depth of the field found in the file.
 *
 * @return Fully populated field object that can later be cast to the
 *         appropriate specific object type.
 */
Field* Field::Read(HANDLE hFile, size_t *bytes, uint8_t *depth) {
	Field *self = NULL;
	uint8_t ucType;
	DWORD dwRead = 0;

	// Get the type of the field to know which class to instantiate.
	if (!::ReadFile(hFile, &ucType, sizeof(uint8_t), &dwRead, NULL)) {
		ThrowError(new ReadError(hFile, *bytes, true));
		return BOLOTA_ERR_NULL;
	}
	*bytes += dwRead;

	// Instantiate the correct field object.
	bolota_type_t type = static_cast<bolota_type_t>(ucType);
	switch (type) {
	case BOLOTA_TYPE_TEXT:
		self = new TextField();
		break;
	case BOLOTA_TYPE_DATE:
		self = new DateField();
		break;
	case BOLOTA_TYPE_ICON:
		self = new IconField();
		break;
	default:
		ThrowError(new UnknownFieldType(hFile, *bytes, true, type));
		goto error_handling;
	}

	// Parse the field.
	*depth = self->ReadField(hFile, bytes);
	if ((*depth == BOLOTA_ERR_UINT8) && BolotaHasError)
		goto error_handling;

	return self;

error_handling:
	if (self)
		delete self;
	self = NULL;
	return BOLOTA_ERR_NULL;
}

/**
 * Reads the entire field from a file.
 *
 * @param hFile File handle to read the field from.
 * @param bytes Pointer to the counter storing the number of bytes read so far.
 *
 * @return Depth of the field found in the file or BOLOTA_ERR_UINT8 if an error
 *         happened.
 */
uint8_t Field::ReadField(HANDLE hFile, size_t *bytes) {
	DWORD dwRead = 0;
	uint8_t depth = 0;
	uint16_t usFieldLength = 0;
	uint16_t usTextLength = 0;

	// Read important bits.
	if (!::ReadFile(hFile, &depth, sizeof(uint8_t), &dwRead, NULL)) {
		ThrowError(new ReadError(hFile, *bytes, true));
		return BOLOTA_ERR_UINT8;
	}
	*bytes += dwRead;
	if (!::ReadFile(hFile, &usFieldLength, sizeof(uint16_t), &dwRead, NULL)) {
		ThrowError(new ReadError(hFile, *bytes, true));
		return BOLOTA_ERR_UINT8;
	}
	*bytes += dwRead;
	if (!::ReadFile(hFile, &usTextLength, sizeof(uint16_t), &dwRead, NULL)) {
		ThrowError(new ReadError(hFile, *bytes, true));
		return BOLOTA_ERR_UINT8;
	}
	*bytes += dwRead;

	// Allocate and read the text from the text from the field.
	char *szText = (char *)malloc((usTextLength + 1) * sizeof(char));
	if (szText == NULL) {
		ThrowError(new SystemError(EMSG("Failed to allocate memory for field ")
			_T("text")));
		return BOLOTA_ERR_UINT8;
	}
	dwRead = 0;
	if (usTextLength > 0) {
		if (!::ReadFile(hFile, szText, usTextLength, &dwRead, NULL)) {
			ThrowError(new ReadError(hFile, *bytes, true));
			return BOLOTA_ERR_UINT8;
		}
	}
	*bytes += dwRead;
	szText[usTextLength] = '\0';
	SetTextOwner(szText);

	return depth;
}

/**
 * Writes the field contents to a file.
 *
 * @param hFile File handle to write the field to.
 *
 * @return Number of bytes written to the file or BOLOTA_ERR_SIZET if an error
 *         occurred during the process.
 */
size_t Field::Write(HANDLE hFile) const {
	size_t ulBytes = 0;
	DWORD dwWritten = 0;

	// Type of the field.
	uint8_t iValue = m_type;
	if (!::WriteFile(hFile, &iValue, sizeof(uint8_t), &dwWritten, NULL)) {
		ThrowError(new WriteError(hFile, ulBytes, true));
		return BOLOTA_ERR_SIZET;
	}
	ulBytes += dwWritten;

	// Depth of the field.
	iValue = Depth();
	if (!::WriteFile(hFile, &iValue, sizeof(uint8_t), &dwWritten, NULL)) {
		ThrowError(new WriteError(hFile, ulBytes, true));
		return BOLOTA_ERR_SIZET;
	}
	ulBytes += dwWritten;

	// Length of data.
	uint16_t usFieldLength = FieldLength();
	if (!::WriteFile(hFile, &usFieldLength, sizeof(uint16_t), &dwWritten,
			NULL)) {
		ThrowError(new WriteError(hFile, ulBytes, true));
		return BOLOTA_ERR_SIZET;
	}
	ulBytes += dwWritten;

	// Length of data.
	uint16_t usTextLength = TextLength();
	if (!::WriteFile(hFile, &usTextLength, sizeof(uint16_t), &dwWritten, NULL)) {
		ThrowError(new WriteError(hFile, ulBytes, true));
		return BOLOTA_ERR_SIZET;
	}
	ulBytes += dwWritten;

	// Data of the field.
	if (m_text) {
		if (!::WriteFile(hFile, m_text->GetMultiByteString(), usTextLength,
				&dwWritten, NULL)) {
			ThrowError(new WriteError(hFile, ulBytes, true));
			return BOLOTA_ERR_SIZET;
		}
		ulBytes += dwWritten;
	}

	return ulBytes;
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                           Getters and Setters                             |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Gets the field type.
 *
 * @return Field type.
 */
bolota_type_t Field::Type() const {
	return m_type;
}

/**
 * Sets the field type.
 *
 * @param type Field type.
 */
void Field::SetType(bolota_type_t type) {
	m_type = type;
}

/**
 * Gets the field depth based on its parent objects.
 *
 * @return Field depth.
 */
uint8_t Field::Depth() const {
	return HasParent() ? (Parent()->Depth() + 1) : 0;
}

/**
 * Checks if the field text exists.
 *
 * @returns TRUE if some text is associated with this field.
 */
bool Field::HasText() const {
	return m_text != NULL;
}

/**
 * Gets the field text.
 *
 * @return Field text if one is available, NULL otherwise.
 */
UString* Field::Text() const {
	return m_text;
}

/**
 * Sets the field's text component.
 *
 * @param mbstr Multi byte string of text to be associated with the field.
 */
void Field::SetText(const char *mbstr) {
	if (HasText()) {
		*m_text = mbstr;
	} else {
		m_text = new UString(mbstr);
	}
}

/**
 * Sets the field's text component.
 *
 * @param wstr Wide character string of text to be associated with the field.
 */
void Field::SetText(const wchar_t *wstr) {
	if (HasText()) {
		*m_text = wstr;
	} else {
		m_text = new UString(wstr);
	}
}

/**
 * Sets the field's text component by taking ownership of the passed string.
 *
 * @param mbstr Multi byte string of text to be owned by the field.
 */
void Field::SetTextOwner(char *mbstr) {
	if (HasText()) {
		m_text->TakeOwnership(mbstr);
	} else {
		m_text = new UString();
		m_text->TakeOwnership(mbstr);
	}
}

/**
 * Sets the field's text component by taking ownership of the passed string.
 *
 * @param wstr Wide character string of text to be owned by the field.
 */
void Field::SetTextOwner(wchar_t *wstr) {
	if (HasText()) {
		m_text->TakeOwnership(wstr);
	} else {
		m_text = new UString();
		m_text->TakeOwnership(wstr);
	}
}

/**
 * Gets the length of the entire field structure when written to a file.
 *
 * @return Length of the entire field structure (including header) in bytes.
 */
uint16_t Field::FieldLength() const {
	return (sizeof(uint8_t) * 2) + sizeof(uint16_t) + (sizeof(uint16_t) +
		TextLength());
}

/**
 * Gets the length of the data part of the field when written to a file.
 *
 * @return Length of the data part of the field structure in bytes.
 */
uint16_t Field::TextLength() const {
	if (m_text == NULL)
		return 0;

	return static_cast<uint16_t>(m_text->Length() * sizeof(char));
}

/*
 * +===========================================================================+
 * |                                                                           |
 * |                              Linked List                                  |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Checks if a parent field exists.
 *
 * @returns TRUE if a parent field is associated with this object.
 */
bool Field::HasParent() const {
	return m_parent != NULL;
}

/**
 * Gets the parent field of this object.
 *
 * @return Parent field.
 */
Field* Field::Parent() const {
	return m_parent;
}

/**
 * Sets the parent field of this object.
 *
 * @param parent   Parent field.
 * @param bPassive Should we not propagate changes to the parent? Set to FALSE
 *                 if we should also set the parent's child property.
 *
 * @return Parent field.
 */
Field* Field::SetParent(Field *parent, bool bPassive) {
	m_parent = parent;
	if (!bPassive && (m_parent != NULL) && (m_parent->Child() != this))
		m_parent->SetChild(this, true);

	return parent;
}

/**
 * Sets the parent field of this object and propagates this change to the
 * parent.
 *
 * @param parent Parent field.
 *
 * @return Parent field.
 */
Field* Field::SetParent(Field *parent) {
	return SetParent(parent, false);
}

/**
 * Checks if a child field exists.
 *
 * @returns TRUE if a child field is associated with this object.
 */
bool Field::HasChild() const {
	return m_child != NULL;
}

/**
 * Gets the child field of this object.
 *
 * @return Child field.
 */
Field* Field::Child() const {
	return m_child;
}

/**
 * Sets the child field of this object.
 *
 * @param child    Child field.
 * @param bPassive Should we not propagate changes to the child? Set to FALSE if
 *                 we should also set the child's parent property.
 *
 * @return Child field.
 */
Field* Field::SetChild(Field *child, bool bPassive) {
	m_child = child;
	if (!bPassive && (m_child != NULL) && (m_child->Parent() != this))
		m_child->SetParent(this, true);

	return child;
}

/**
 * Sets the child field of this object and propagates this change to the child.
 *
 * @param child Child field.
 *
 * @return Child field.
 */
Field* Field::SetChild(Field *child) {
	return SetChild(child, false);
}

/**
 * Checks if a previous field exists.
 *
 * @returns TRUE if a previous field is associated with this object.
 */
bool Field::HasPrevious() const {
	return m_prev != NULL;
}

/**
 * Gets the previous field in the linked list.
 *
 * @return Previous field.
 */
Field* Field::Previous() const {
	return m_prev;
}

/**
 * Sets the previous field in the linked list.
 *
 * @param prev     Previous field.
 * @param bPassive Set to FALSE if the next property of the previous field must
 *                 also be changed to this one. This will also make us a child
 *                 of the previous' parent.
 *
 * @return Previous field.
 */
Field* Field::SetPrevious(Field *prev, bool bPassive) {
	// Set the previous field.
	m_prev = prev;

	// Update relatives.
	if (!bPassive && (prev != NULL)) {
		SetParent(prev->Parent(), true);
		if (prev->Next() != this)
			prev->SetNext(this, true);
	}

	return prev;
}

/**
 * Sets the previous field in the linked list.
 *
 * @param prev Previous field.
 *
 * @return Previous field.
 */
Field* Field::SetPrevious(Field *prev) {
	return SetPrevious(prev, false);
}

/**
 * Checks if a next field exists.
 *
 * @returns TRUE if a next field is associated with this object.
 */
bool Field::HasNext() const {
	return m_next != NULL;
}

/**
 * Gets the next field in the linked list.
 *
 * @return Next field.
 */
Field* Field::Next() const {
	return m_next;
}

/**
 * Sets the next field in the linked list.
 *
 * @param next     Next field.
 * @param bPassive Set to FALSE if the previous property of the next field must
 *                 also be changed to this one. This will also make next a child
 *                 of our parent.
 *
 * @return Next field.
 */
Field* Field::SetNext(Field *next, bool bPassive) {
	// Set the next field.
	m_next = next;

	// Update relatives.
	if (!bPassive && (next != NULL)) {
		next->SetParent(Parent(), true);
		if (next->Previous() != this)
			next->SetPrevious(this, true);
	}

	return next;
}

/**
 * Sets the next field in the linked list.
 *
 * @param next Next field.
 *
 * @return Next field.
 */
Field* Field::SetNext(Field *next) {
	return SetNext(next, false);
}

/**
 * Checks if this field is the first child of its parent.
 *
 * @return TRUE if the is the first child of its parent.
 */
bool Field::IsFirstChild() const {
	return HasParent() && (Parent()->Child() == this);
}

/**
 * Checks if this field is the last element of the document.
 *
 * @return TRUE if the is the field is the last field in the document.
 */
bool Field::IsDocumentLast() const {
	return (!HasNext() && !HasParent()) || (HasParent() &&
		Parent()->IsDocumentLast());
}

/**
 * Checks the consistency of the relationships of the relationships of a the
 * field. In other words, it checks if the field related to this actually point
 * to it in the correct manner (next's previous points to us, previous's next
 * points to us, etc.)
 *
 * @return A ConsistencyError if an inconsistency is found, NULL otherwise.
 */
Error* Field::CheckConsistency() {
	// Check if the first child of a parent has a previous field.
	if (HasParent() && (Parent()->Child() == this) && HasPrevious()) {
		return ThrowError(new ConsistencyError(this, NULL, Previous(),
			EMSG("First Child Previous")));
	}

	// Are we the child's parent?
	if (HasChild() && (Child()->Parent() != this)) {
		return ThrowError(new ConsistencyError(this, this, Child()->Parent(),
			EMSG("Child Parent")));

		// Is the child holding a previous?
		if (Child()->HasPrevious()) {
			return ThrowError(new ConsistencyError(this, NULL,
				Child()->Previous(), EMSG("Child Previous")));
		}
	}

	// Are we the previous's next?
	if (HasPrevious() && (Previous()->Next() != this)) {
		return ThrowError(new ConsistencyError(this, this, Previous()->Next(),
			EMSG("Previous Next")));
	}

	// Are we the next's previous?
	if (HasNext() && (Next()->Previous() != this)) {
		return ThrowError(new ConsistencyError(this, this, Next()->Previous(),
			EMSG("Next Previous")));
	}

	return NULL;
}
