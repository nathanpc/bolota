/**
 * ConsistencyError.cpp
 * Error that is thrown whenever there's an inconsistency between related
 * fields.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "ConsistencyError.h"

using namespace Bolota;

/**
 * Initializes the consistency error object.
 * 
 * @param reference    Field that was checked or compared against.
 * @param expected     What were we expected to find there.
 * @param found        What was actually found in its place.
 * @param relationship The kind of the relationship that was tested.
 */
ConsistencyError::ConsistencyError(Field* reference, Field* expected,
								   Field* found, const TCHAR* relationship) :
	Error() {
	// Set class properties.
	fieldReference = reference;
	fieldExpected = expected;
	fieldFound = found;
	strRelationship = relationship;

	// Create a temporary buffer for building up a complex string.
	TCHAR szBuffer[255];
	szBuffer[254] = _T('\0');

	// Build up our message string.
	tstring strMessage = _T("An inconsistency was found between ")
		_T("fields ");
	_sntprintf(szBuffer, 254, _T("%p (reference), %p (expected), and ")
		_T("%p (found) in relation to their %s property.") LINENDT
		LINENDT, reference, expected, found, relationship);
	strMessage += szBuffer;
	_sntprintf(szBuffer, 254, _T("Reference (%p): %s") LINENDT,
		reference, reference->Text()->GetNativeString());
	strMessage += szBuffer;
	_sntprintf(szBuffer, 254, _T("%s Expected (%p): %s") LINENDT,
		relationship, expected, (expected == NULL) ? NULL :
		expected->Text()->GetNativeString());
	strMessage += szBuffer;
	_sntprintf(szBuffer, 254, _T("%s Found (%p): %s"),
		relationship, found, (found == NULL) ? NULL :
		found->Text()->GetNativeString());
	strMessage += szBuffer;

	// Substitute the error message string.
	if (m_message)
		free(m_message);
	m_message = _tcsdup(strMessage.c_str());
}
