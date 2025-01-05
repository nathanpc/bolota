/**
 * Error.cpp
 * Error handling and reporting class.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Error.h"

using namespace Bolota;

/**
 * Constructs a blank error object.
 */
Error::Error() {
	m_prev = ErrorStack;
	m_message = NULL;
}

/**
 * Builds up the error object with a message string and uses the current error
 * in the stack as its previous.
 *
 * @param message Message to be displayed.
 */
Error::Error(const TCHAR *message) {
	m_prev = ErrorStack;
	m_message = _tcsdup(message);
}

/**
 * Builds up the error object with a message string and a previous error.
 *
 * @param prev    Previous error that was thrown.
 * @param message Message to be displayed.
 */
Error::Error(Error *prev, const TCHAR *message) {
	m_prev = prev;
	m_message = _tcsdup(message);
}

/**
 * Frees up any resources allocated by the object.
 */
Error::~Error() {
	// Free up the message.
	if (m_message)
		free(m_message);
	m_message = NULL;

	// Ensure that the error stack has a valid pointer.
	if (ErrorStack == this)
		ErrorStack = m_prev;
}

/**
 * Throws a brand new error and surfaces it up the stack.
 *
 * @param message Message to be displayed.
 *
 * @return The error that was thrown.
 */
Error* Error::Throw(const TCHAR *message) {
	ErrorStack = new Error(ErrorStack, message);
	return ErrorStack;
}

/**
 * Puts an error object on the top of the stack.
 *
 * @warning This method will change the previous reference of the object to the
 *          current error object in the stack.
 *
 * @param message Message to be displayed.
 *
 * @return The error that was thrown.
 */
Error* Error::Throw(Error *error) {
	error->m_prev = ErrorStack;
	ErrorStack = error;

	return ErrorStack;
}

/**
 * Gets the error message in the native string format.
 *
 * @return Error message in the native string format.
 */
const TCHAR* Error::Message() {
	return this->m_message;
}

/**
 * Destroys the current error object and gets its previous.
 *
 * @warning This will delete the reference to the current error object.
 *
 * @return Previous error object if it exists or NULL.
 */
Error* Error::Pop() {
	delete this;
	return ErrorStack;
}

/**
 * Clears the entire contents of the error stack.
 */
void Error::Clear() {
	while (BolotaHasError)
		ErrorStack->Pop();
}
