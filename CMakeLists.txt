cmake_minimum_required(VERSION 3.25)
project(Cloud)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(OS_DIR "win")
elseif (LINUX)
    set(OS_DIR "linux")
elseif (APPLE)
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

#Set both libraries and executables to be in the same path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE MAIN_SOURCES #My code
    src/*.cpp
    src/*.c
    include/*.hpp #Used by IDEs to account for header files too.
    include/*.h
	third-party/*.hpp
	third-party/*.h
)

#Now remove shared libraries from MAIN_SOURCES
list(REMOVE_ITEM MAIN_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/opengl_renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/vulkan_renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/dx12_renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/metal_renderer.cpp"
)

file(GLOB THIRD_PARTY_SOURCES
	third-party/tracy/TracyClient.cpp
	third-party/*.cpp
	third-party/*.c
)

add_executable(Cloud ${MAIN_SOURCES} ${THIRD_PARTY_SOURCES})

set_target_properties(Cloud PROPERTIES #Set the working directory to be the executable path, for IDEs
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Cloud>"
)

target_compile_definitions(Cloud PRIVATE #Useful for loading shared libraries
    SHARED_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
    SHARED_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_SOURCES} ${THIRD_PARTY_SOURCES}) #Make them show in IDEs

#Compiler options
option(SHOW_CONSOLE "Show console" ON) #Or else print only on file
option(TRACY_ENABLED "Enable tracy" ON) #Enable profiling
option(DEBUG_BUILD "Enable debug build" ON) #Enable debug symbols

if (SHOW_CONSOLE)
	target_compile_definitions(Cloud PRIVATE SHOW_CONSOLE)
    message(STATUS "SHOW_CONSOLE option is enabled")
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
    message(STATUS "TRACY_ENABLED option is enabled")
endif ()

if (DEBUG_BUILD AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Cloud PRIVATE -g)
    message(STATUS "DEBUG_BUILD option is enabled")
endif()

#Global include directiories
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/third-party
${CMAKE_CURRENT_SOURCE_DIR}/third-party/tracy)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)

set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

list(APPEND IMGUI_SOURCES
	${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	#CURRENTLY DISABLE THIS UNTIL I IMPORT VULKAN IN THE REPO ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm
)

if (NOT APPLE)
	list(REMOVE_ITEM IMGUI_SOURCES ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
	message(STATUS "removed imgui metal implementation since the target platform is not apple")
endif()

if (NOT WIN32)
    list(REMOVE_ITEM IMGUI_SOURCES ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)
    message(STATUS "removed imgui dx12 implementation since the target platform is not windows")
endif()

add_library(ImGui STATIC ${IMGUI_SOURCES})

target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(ImGui PUBLIC
	glfw
)

if (WIN32)
    target_link_libraries(Cloud PRIVATE glfw ImGui opengl32 gdi32 user32 shell32)
elseif (APPLE)
    target_link_libraries(Cloud PRIVATE glfw ImGui)
else ()
    target_link_libraries(Cloud PRIVATE glfw ImGui GL)
endif ()

### == RENDERERS == ###
#Compile OpenGL for every system
add_library(OpenGLRenderer SHARED
${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/opengl_renderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/third-party/glad.c
)

target_compile_definitions(OpenGLRenderer PRIVATE BUILD_RENDERER)

#Compile vulkan everywhere but non on macOS
if (NOT APPLE)
	add_library(VulkanRenderer SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/vulkan_renderer.cpp)
	target_compile_definitions(VulkanRenderer PRIVATE BUILD_RENDERER)
endif ()

#Compile DX12 if we are on windows
if (WIN32)
	add_library(DX12Renderer SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/dx12_renderer.cpp)
	target_compile_definitions(DX12Renderer PRIVATE BUILD_RENDERER)
	target_link_libraries(DX12Renderer PRIVATE d3d12 dxgi)
endif ()

#Compile Metal if we are on MacOS
if (APPLE)
	add_library(MetalRenderer SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/subsystems/renderers/metal_renderer.cpp)
	target_compile_definitions(MetalRenderer PRIVATE BUILD_RENDERER)
	target_link_libraries(MetalRenderer PRIVATE "-framework Metal" "-framework Foundation")
endif ()

set_target_properties(Cloud OpenGLRenderer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
if (TARGET VulkanRenderer)
    set_target_properties(VulkanRenderer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif ()

if (TARGET DX12Renderer)
    set_target_properties(DX12Renderer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif ()

if (TARGET MetalRenderer)
    set_target_properties(MetalRenderer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif ()

#Set the runtime path to be the executable path
set_target_properties(Cloud PROPERTIES
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
)