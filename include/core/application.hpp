#pragma once
#include <core/startup_settings.hpp>

//These are the REQUIRED opengl versions that the driver must support. If the driver doesn't support this, the app won't launch.
#define OPENGL_MAJOR_REQUIRED 3
#define OPENGL_MINOR_REQUIRED 3

namespace Application
{
	bool Launch();
	
	bool IsModuleInitialized(int moduleFlag);

	const StartupSettings& GetSettings();
}
