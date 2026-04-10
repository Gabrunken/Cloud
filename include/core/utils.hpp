#pragma once

#if defined(_MSC_VER)
// Microsoft Visual C++
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
// GCC, Clang, and compatible compilers
#define FORCE_INLINE inline __attribute__((always_inline))
#else
// Fallback for unknown compilers
#define FORCE_INLINE inline
#endif

#include <core/logger.hpp>

//Asserts the condition and pushes a message into the logger (it must be initialized)
//All failed assertions will push a FATAL message.
FORCE_INLINE void CloudAssert(bool condition, const std::string& caller, const std::string& message)
{
	if (!condition)
	{
		Logger::PushMessage(caller, "FAILED ASSERTION -- " + message, Logger::Fatal);
		exit(1);
	}
}

#ifdef _WIN32
	#define API_EXPORT __declspec(dllexport)
	#define API_IMPORT __declspec(dllimport)
#else
	#define API_EXPORT __attribute__((visibility("default")))
	#define API_IMPORT
#endif