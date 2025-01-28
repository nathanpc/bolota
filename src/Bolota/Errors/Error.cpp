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
	m_prev = ErrorStack::Top();
	m_message = NULL;
}

/**
 * Builds up the error object with a message string and uses the current error
 * in the stack as its previous.
 *
 * @param message Message to be displayed.
 */
Error::Error(const TCHAR *message) {
	m_prev = ErrorStack::Top();
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
}

/**
 * Throws a brand new error and surfaces it up the stack.
 *
 * @param message Message to be displayed.
 *
 * @return The error that was thrown.
 */
Error* Error::Throw(const TCHAR *message) {
	return Throw(new Error(NULL, message));
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
	return ErrorStack::Instance()->Push(error);
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
 * Sets the current error as the previous of the error passed as an argument.
 * 
 * @warning This will not change the error stack, possibly corrupting it.
 * 
 * @param error Next error in the stack.
 * 
 * @return The error that was passed as the argument.
 */
Error* Error::Push(Error* error) {
	error->m_prev = this;
	return error;
}

/**
 * Destroys the current error object and gets its previous.
 * 
 * @warning This will not change the error stack, possibly corrupting it.
 *
 * @return Previous error object if it exists or NULL.
 */
Error* Error::Pop() {
	Error* prev = m_prev;
	delete this;

	return prev;
}

/**
 * Initializes the error stack singleton object.
 */
ErrorStack::ErrorStack() {
	m_stack = NULL;
}

/**
 * Deallocates the internal error stack.
 */
ErrorStack::~ErrorStack() {
	// Clear the stack and invalidate our instance.
	Clear();
	Instance(true);
}

/**
 * Internal global singleton instance allocator and invalidator.
 * 
 * @param invalidate Should we invalidate our global instance?
 * 
 * @return Global singleton instance.
 */
ErrorStack* ErrorStack::Instance(bool invalidate) {
	static ErrorStack* stack;

	// Should we invalidate?
	if (invalidate) {
		stack = NULL;
		return NULL;
	}

	// Allocate our global instance if needed.
	if (stack == NULL)
		stack = new ErrorStack();

	return stack;
}

/**
 * Gets the global instance of the error stack singleton.
 * 
 * @return Global error stack object.
 */
ErrorStack* ErrorStack::Instance() {
	return Instance(false);
}

/**
 * Gets the topmost item of the error stack.
 * 
 * @return Topmost item of the error stack.
 */
Error* ErrorStack::Top() {
	return Instance()->m_stack;
}

/**
 * Pushes an error to the top of the error stack.
 *
 * @param error New error to be the top of the error stack.
 * 
 * @return The new top of the stack.
 */
Error* ErrorStack::Push(Error* error) {
	m_stack = m_stack->Push(error);
	return m_stack;
}

/**
 * Destroys the current error object at the top of the stack and gets its
 * previous.
 *
 * @warning This will delete the reference to the current error object.
 *
 * @return Previous error object if it exists or NULL.
 */
Error* ErrorStack::Pop() {
	m_stack = m_stack->Pop();
	return m_stack;
}

/**
 * Clears the entire contents of the error stack.
 */
void ErrorStack::Clear() {
	while (m_stack != NULL)
		Pop();
}
