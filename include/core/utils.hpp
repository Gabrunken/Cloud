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