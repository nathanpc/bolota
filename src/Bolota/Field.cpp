/**
 * Field.cpp
 * Field object abstraction of a Bolota document.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Field.h"

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
 * Constructs a field object with a predefined type and a parent field.
 *
 * @param parent Parent field.
 * @param type   Type of the field.
 */
Field::Field(Field *parent, bolota_type_t type) {
	Initialize(type, NULL, parent, NULL, NULL, NULL);
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
	SetParent(parent);
	SetChild(child);
	SetPrevious(prev);
	SetNext(next);
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

/*
 * +===========================================================================+
 * |                                                                           |
 * |                             File Operations                               |
 * |                                                                           |
 * +===========================================================================+
 */

/**
 * Writes the field contents to a file.
 *
 * @param hFile File handle to write the field to.
 *
 * @return Number of bytes written to the file.
 */
size_t Field::Write(HANDLE hFile) const {
	size_t ulBytes = 0;
	DWORD dwWritten = 0;

	// Type of the field.
	uint8_t iValue = m_type;
	::WriteFile(hFile, &iValue, sizeof(uint8_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// Depth of the field.
	iValue = Depth();
	::WriteFile(hFile, &iValue, sizeof(uint8_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// Length of data.
	uint16_t usLength = DataLength();
	::WriteFile(hFile, &usLength, sizeof(uint16_t), &dwWritten, NULL);
	ulBytes += dwWritten;

	// Data of the field.
	if (m_text) {
		::WriteFile(hFile, m_text->GetMultiByteString(), DataLength(),
			&dwWritten, NULL);
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
uint16_t Field::Length() const {
	return (sizeof(uint8_t) * 2) + sizeof(uint16_t) + DataLength();
}

/**
 * Gets the length of the data part of the field when written to a file.
 *
 * @return Length of the data part of the field structure in bytes.
 */
uint16_t Field::DataLength() const {
	if (m_text == NULL)
		return 0;

	return m_text->Length() * sizeof(char);
}

/**
 * Generates a struct with the information contained in the object.
 *
 * @warning Do not free or manipulate the text field. It's shared internally
 *          with another object.
 *
 * @return A struct with the information from the object.
 */
bolota_field_t Field::Struct() const {
	bolota_field_t field;

	// Initialize the structure.
	field.type = Type();
	field.depth = Depth();
	field.length = static_cast<uint16_t>(Text()->Length());
	field.text = const_cast<char *>(Text()->GetMultiByteString());

	return field;
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
 * @param bPassive Should we not propagate changes to the parent? Set to FALSE
 *                 if we should also set the parent's child property.
 *
 * @param parent Parent field.
 */
void Field::SetParent(Field *parent, bool bPassive) {
	m_parent = parent;
	SetPrevious(NULL);
	if (!bPassive && (m_parent != NULL) && (m_parent->Child() != this))
		m_parent->SetChild(this);
}

/**
 * Sets the parent field of this object and propagates this change to the
 * parent.
 *
 * @param parent Parent field.
 */
void Field::SetParent(Field *parent) {
	SetParent(parent, false);
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
 * @param bPassive Should we not propagate changes to the child? Set to FALSE if
 *                 we should also set the child's parent property.
 *
 * @param child Child field.
 */
void Field::SetChild(Field *child, bool bPassive) {
	m_child = child;
	if (!bPassive && (m_child != NULL) && (m_child->Parent() != this))
		m_child->SetParent(this);
}

/**
 * Sets the child field of this object and propagates this change to the child.
 *
 * @param child Child field.
 */
void Field::SetChild(Field *child) {
	SetChild(child, false);
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
 * @param prev Previous field.
 */
void Field::SetPrevious(Field *prev) {
	// Ensure we have the same parent.
	//if ((prev != NULL) && (Parent() != prev->Parent()))
	//	m_parent = prev->Parent();

	// Set the previous field.
	m_prev = prev;
	if ((m_prev != NULL) && (m_prev->Next() != this))
		m_prev->SetNext(this);
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
 * @param next Next field.
 */
void Field::SetNext(Field *next) {
	// Ensure we have the same parent.
	//if ((next != NULL) && (Parent() != next->Parent()))
	//	m_parent = next->Parent();

	// Set the next field.
	m_next = next;
	if ((m_next != NULL) && (m_next->Previous() != this))
		m_next->SetPrevious(this);
}
