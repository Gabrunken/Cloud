#pragma once
#include <string>

using LibraryHandle = void*;

LibraryHandle LoadSharedLibrary(const std::string& path);
void* GetLibrarySymbol(LibraryHandle handle, const std::string& symbolName);
void UnloadSharedLibrary(LibraryHandle handle);
