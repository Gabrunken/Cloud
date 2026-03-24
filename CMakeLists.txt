cmake_minimum_required(VERSION 3.10)
project(Cloud)

#Or else print only on file
option(SHOW_CONSOLE "Show console" ON)

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

message(STATUS "Configurazione Cloud per: ${OS_DIR} / ${ARCH_DIR}")

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.c
    include/*.hpp
    include/*.h
)

add_executable(Cloud ${SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if (SHOW_CONSOLE)
	target_compile_definitions(Cloud PRIVATE SHOW_CONSOLE)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")
target_link_directories(Cloud PRIVATE ${LIB_PATH})

if (OS_DIR STREQUAL "win")
    target_link_libraries(Cloud PRIVATE glfw3 opengl32 gdi32 user32 shell32)
else ()
    target_link_libraries(Cloud PRIVATE glfw3)
endif ()