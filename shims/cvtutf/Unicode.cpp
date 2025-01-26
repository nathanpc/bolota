/**
 * Unicode.cpp
 * A wrapper for Unicode's CVTUTF string conversion library.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Unicode.h"

#include <stdlib.h>
#include <string.h>

#include "ConvertUTF.h"

namespace Unicode {

/**
 * Margin used to allocate a buffer that can hold the conversion result, defined
 * as: Allocation Margin = Original Buffer + Margin.
 */
#define UTF_ALLOC_MARGIN 1.3f


/**
 * Checks if the wchar_t is the same size as UTF-16 (2 bytes).
 * 
 * @warning This check is REQUIRED to be done by your application once at
 *          startup. If it fails you have to abort since all other operations
 *          assume that wchar_t is 2 bytes long.
 * 
 * @return TRUE if wchar_t is properly defined as 2 bytes long. FALSE otherwise.
 */
bool WideCharIsUTF16() {
	return sizeof(wchar_t) == sizeof(UTF16);
}

/**
 * Converts a multi-byte string (UTF-8) to a wide-character string (UTF-16).
 * 
 * @warning This function allocates memory dynamically that must be free'd.
 * 
 * @param mbstr UTF-8 string to be converted.
 * @param wstr  Pointer to a newly allocated UTF-16 string.
 * 
 * @return TRUE if the conversion was successful, FALSE otherwise.
 */
bool MultiByteToWideChar(const char* mbstr, wchar_t** wstr) {
	const char* szInputStart = mbstr;
	const char* szInputEnd = NULL;
	wchar_t* szOutputStart = NULL;
	wchar_t* szOutputEnd = NULL;

	// Measure the input buffer and get the location of the NUL terminator.
	size_t len = strlen(szInputStart);
	szInputEnd = szInputStart + len;

	// Allocate the new buffer and get its end target (NUL terminator).
	szOutputStart = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	if (szOutputStart == NULL)
		return false;
	szOutputEnd = szOutputStart + len;

	// Perform the conversion.
	const UTF8* szInput = (const UTF8*)szInputStart;
	UTF16* szOutput = (UTF16*)szOutputStart;
	ConversionResult res = ConvertUTF8toUTF16(&szInput, (const UTF8*)szInputEnd,
		&szOutput, (UTF16*)szOutputEnd, lenientConversion);
	if (res != conversionOK) {
		free(szOutputStart);
		*wstr = NULL;
		return false;
	}
	*szOutput = (UTF16)L'\0';

	// Trim output buffer size if needed.
	size_t lenOutput = szOutput - (UTF16*)szOutputStart + 1;
	if (lenOutput < len) {
		szOutputStart = (wchar_t*)realloc(szOutputStart, lenOutput *
			sizeof(wchar_t));
	}

	// Set the output pointer and return.
	*wstr = szOutputStart;
	return true;
}

} // namespace Unicode
