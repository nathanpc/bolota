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
 * @param parent Parent field.
 */
void Field::SetParent(Field *parent) {
	m_parent = parent;
	if ((m_parent != NULL) && (m_parent->Child() != this))
		m_parent->SetChild(this);
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
 * @param child Child field.
 */
void Field::SetChild(Field *child) {
	m_child = child;
	if ((m_child != NULL) && (m_child->Parent() != this))
		m_child->SetParent(this);
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
	m_next = next;
	if ((m_next != NULL) && (m_next->Previous() != this))
		m_next->SetPrevious(this);
}
