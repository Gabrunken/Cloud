#include <platform/shared_library.hpp>

#ifdef SHOW_CONSOLE
    #include <iostream>
#endif

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

LibraryHandle LoadSharedLibrary(const std::string& path)
{
    #ifdef _WIN32
    HMODULE handle = LoadLibraryA(path.c_str());
    if (!handle)
    {
        #ifdef SHOW_CONSOLE
        std::cerr << "Could not load dll at path: " << path << std::endl;
        #endif
        return nullptr;
    }

    return static_cast<LibraryHandle>(handle);

    #else

    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
    {
        #ifdef SHOW_CONSOLE
        std::cerr << "Could not load shared library at path: " << path << "\nDetails: " << dlerror() << std::endl;
        #endif
    }

    return static_cast<LibraryHandle>(handle);
    #endif
}

void* GetLibrarySymbol(LibraryHandle handle, const std::string& symbolName)
{
    if (!handle)
    {
        #ifdef SHOW_CONSOLE
        std::cerr << "Failed to get a shared library symbol: the passed handle was null\n";
        #endif
        return nullptr;
    }

    #ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), symbolName.c_str()));
    #else
    return dlsym(handle, symbol.c_str());
    #endif
}

void UnloadSharedLibrary(LibraryHandle handle)
{
    if (!handle)
    {
        #ifdef SHOW_CONSOLE
        std::cerr << "Failed to unload a shared library, the passed handle was null\n";
        #endif
        return;
    }

    #ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle));
    #else
    dlclose(handle);
    #endif
}