/**
 * ConfigManager.h
 * Manages the application's configuration options.
 *
 * @author Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_SETTINGS_CONFIGMANAGER_H
#define _UTILITIES_SETTINGS_CONFIGMANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>
#include <stdint.h>

#include "Setting.h"

/**
 * Number of available settings to use.
 */
#define BOLOTA_SETTINGS_NUM 3

/**
 * Gets a Setting object from the manager.
 *
 * @param type  Setting data type.
 * @param index Index of which setting to get.
 *
 * @return Specified setting object.
 */
#define Settings_Get(type, index) (\
	static_cast<Setting<type>*>(ConfigManager::Instance()->Get(index)))

/**
 * Gets the value of a setting from the manager.
 *
 * @param type  Setting data type.
 * @param index Index of which setting to get.
 *
 * @return Specified setting value.
 */
#define Settings_GetValue(type, index) (Settings_Get(type, index)->Value())

/**
 * Sets the value of a setting.
 *
 * @param type  Setting data type.
 * @param index Index of which setting we want to set the value of.
 * @param value New value to put in the setting object.
 *
 * @return Specified setting object.
 */
#define Settings_SetValue(type, index, value) ( \
	Settings_Get(type, index)->SetValue(value))

/**
 * Sets the value of a setting and save it instantly.
 *
 * @param type  Setting data type.
 * @param index Index of which setting we want to set the value of.
 * @param value New value to put in the setting object.
 *
 * @return Specified setting object.
 */
#define Settings_SaveValue(type, index, value) ( \
	Settings_SetValue(type, index, value)->Save())

namespace Settings {

	/**
	 * Manages the application's configuration options.
	 */
	class ConfigManager {
	public:
		enum SettingIndex {
			WindowWidth = 0,
			WindowHeight,
			TreeViewStyleFlags
		};

	private:
		BaseSetting** m_settings;

		// Singleton stuff.
		ConfigManager();
		ConfigManager(ConfigManager const&);
		void operator=(ConfigManager const&);

	public:
		// Constructors and destructors.
		static ConfigManager* Instance();
		virtual ~ConfigManager();

		// Getters
		BaseSetting* Get(SettingIndex index) const;
	};

} // Settings

#endif // _UTILITIES_SETTINGS_CONFIGMANAGER_H
