/**
 * Error.cpp
 * Exception type class for systems without exception handling.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Error.h"

#ifndef NOEXCEPT
	#include <stdexcept>
#endif // !NOEXCEPT

using namespace Bolota;

/**
 * Constructs a blank error object.
 */
Error::Error() {
	m_message = NULL;
}

/**
 * Builds up the error object with a multibyte character string.
 *
 * @param message Message to be displayed in the error.
 */
Error::Error(const char *message) {
	m_message = new UString(message);
}

/**
 * Builds up the error object with a wide-character string.
 *
 * @param message Message to be displayed in the error.
 */
Error::Error(const wchar_t *message) {
	m_message = new UString(message);
}

/**
 * Frees up any resources allocated by the object.
 */
Error::~Error() {
	if (m_message)
		delete m_message;
	m_message = NULL;
}

/**
 * Gets the message associated with the error.
 *
 * @return Message associated with the error.
 */
UString* Error::Message() const {
	return this->m_message;
}

/**
 * Throws an exception based on the error.
 */
void Error::Throw() const {
#ifndef NOEXCEPT
	throw std::exception(m_message->GetMultiByteString());
#endif // NOEXCEPT
}
