/**
 * wce_string.cpp
 * Recreates some missing standard library string functions.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "wce_string.h"

#include <string.h>

#ifdef UNDER_CE

/**
 * Duplicates a string.
 *
 * @warning This function allocates memory dynamically.
 *
 * @param str String to be duplicated.
 *
 * @return Newly allocated duplicated string.
 */
char *_strdup(const char *str) {
	char *buf = (char *)malloc((strlen(str) + 1) * sizeof(char));
	strcpy(buf, str);
	return buf;
}

#endif // UNDER_CE
