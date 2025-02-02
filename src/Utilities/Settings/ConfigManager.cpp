/**
 * ConfigManager.cpp
 * Manages the application's configuration options.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#include "ConfigManager.h"

#include <stdlib.h>
#include <windows.h>

#include "../../Bolota/Errors/Error.h"
#include "Setting.h"

using namespace Settings;

/**
 * Initializes the configuration manager.
 */
ConfigManager::ConfigManager() {
	// Initialize internals.
	m_settings = (BaseSetting**)malloc(BOLOTA_SETTINGS_NUM *
		sizeof(BaseSetting*));
	if (m_settings == NULL) {
		ThrowError(EMSG("Failed to allocate settings array"));
		return;
	}

	// Populate the settings array.
	m_settings[WindowWidth] = new Setting<DWORD>(_T("WindowWidth"),
		_T("Window width"), REG_DWORD, (DWORD)600);
	m_settings[WindowHeight] = new Setting<DWORD>(_T("WindowHeight"),
		_T("Window height"), REG_DWORD, (DWORD)400);
}

/**
 * Cleans up all the memory allocated by the configuration manager.
 */
ConfigManager::~ConfigManager() {
	// Free our settings array.
	if (m_settings) {
		for (uint8_t i = 0; i < BOLOTA_SETTINGS_NUM; ++i)
			delete m_settings[i];
		free(m_settings);
		m_settings = NULL;
	}
}

/**
 * Gets the global instance of the configuration manager singleton.
 *
 * @return Global configuration manager object.
 */
ConfigManager* ConfigManager::Instance() {
	static ConfigManager* manager;
	if (manager == NULL)
		manager = new ConfigManager();

	return manager;
}
