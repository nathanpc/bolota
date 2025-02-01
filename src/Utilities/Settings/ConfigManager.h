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
#define BOLOTA_SETTINGS_NUM 2

namespace Settings {

	/**
	 * Manages the application's configuration options.
	 */
	class ConfigManager {
	public:
		enum SettingIndex {
			WindowWidth = 0,
			WindowHeight
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
		template <typename T>
		T* Get(SettingIndex index) const;
	};

} // Settings

#endif // _UTILITIES_SETTINGS_CONFIGMANAGER_H
