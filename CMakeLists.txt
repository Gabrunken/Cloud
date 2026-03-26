cmake_minimum_required(VERSION 3.10)
project(Cloud)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_DIR "win")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_DIR "linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OS_DIR "macos")
else ()
    message(FATAL_ERROR "Unknown OS: ${CMAKE_SYSTEM_NAME}")
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    set(ARCH_DIR "x64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86)$")
    set(ARCH_DIR "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|ARM64)$")
    set(ARCH_DIR "ARM64")
else ()
    message(FATAL_ERROR "Architecture not supported: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

message(STATUS "Building for: ${OS_DIR} / ${ARCH_DIR}")

file(GLOB_RECURSE MAIN_SOURCES #My code
    src/*.cpp
    src/*.c
    include/*.hpp #Used by IDEs to account for header files too.
    include/*.h
	third-party/*.hpp
	third-party/*.h
)

file(GLOB THIRD_PARTY_SOURCES
	third-party/tracy/TracyClient.cpp
	third-party/*.cpp
	third-party/*.c
)

add_executable(Cloud ${MAIN_SOURCES} ${THIRD_PARTY_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_SOURCES} ${THIRD_PARTY_SOURCES}) #Make them show in IDEs

#Compiler macros
option(SHOW_CONSOLE "Show console" ON) #Or else print only on file
option(TRACY_ENABLED "Enable tracy" ON) #Enable profiling

if (SHOW_CONSOLE)
	target_compile_definitions(Cloud PRIVATE SHOW_CONSOLE)
else ()
    if (OS_DIR STREQUAL "win")
        set_target_properties(Cloud PROPERTIES WIN32_EXECUTABLE TRUE)

        if (MSVC)
            target_link_options(Cloud PRIVATE "/ENTRY:mainCRTStartup") #Change entry point from WinMain to classic int main()
        endif()
    endif()
endif ()

if (TRACY_ENABLED)
	target_compile_definitions(Cloud PRIVATE TRACY_ENABLED)
endif ()

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include 
${CMAKE_CURRENT_SOURCE_DIR}/third-party
${CMAKE_CURRENT_SOURCE_DIR}/third-party/tracy)

set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")
target_link_directories(Cloud PRIVATE ${LIB_PATH})

if (OS_DIR STREQUAL "win")
    target_link_libraries(Cloud PRIVATE glfw3 opengl32 gdi32 user32 shell32)
else ()
    target_link_libraries(Cloud PRIVATE glfw3)
endif ()