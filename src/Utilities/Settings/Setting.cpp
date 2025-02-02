/**
 * Setting.cpp
 * Abstraction over a single setting in the configuration manager.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Setting.h"

using namespace Settings;

#include <stdlib.h>


/**
 * Initializes the base settings object.
 * 
 * @param szKey         Key used when storing the setting.
 * @param szDescription Human-friendly description of the setting.
 * @param dwType        Windows registry value type.
 */
BaseSetting::BaseSetting(LPCTSTR szKey, LPCTSTR szDescription, DWORD dwType) {
	m_szKey = _tcsdup(szKey);
	m_szDescription = _tcsdup(szDescription);
	m_dwType = dwType;
}

/**
 * Frees up everything related to the base settings.
 */
BaseSetting::~BaseSetting() {
	// Free the key.
	if (m_szKey)
		free(m_szKey);
	m_szKey = NULL;

	// Free the description.
	if (m_szDescription)
		free(m_szDescription);
	m_szDescription = NULL;
}

/**
 * Gets the key used when storing the setting.
 * 
 * @return Setting key used for storage.
 */
LPCTSTR BaseSetting::Key() const {
	return this->m_szKey;
}

/**
 * Gets the human-friendly description of the setting.
 *
 * @return Human-friendly description.
 */
LPCTSTR BaseSetting::Description() const {
	return this->m_szDescription;
}

/**
 * Gets the Windows registry value type of the setting.
 * 
 * @return Windows registry value type.
 */
DWORD BaseSetting::Type() const {
	return this->m_dwType;
}
