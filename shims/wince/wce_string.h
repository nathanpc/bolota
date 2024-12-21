/**
 * wce_string.h
 * Recreates some missing standard library string functions.
 * 
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _SHIMS_WINCE_STRING_H
#define _SHIMS_WINCE_STRING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#ifdef UNDER_CE

char *_strdup(const char *str);

#endif // UNDER_CE

#endif // _SHIMS_WINCE_STRING_H
