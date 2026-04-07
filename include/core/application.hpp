#pragma once
#include <core/startup_settings.hpp>

namespace Application
{
	bool Initialize(const char* settingsFilePath);

	const StartupSettings& GetSettings();
}
