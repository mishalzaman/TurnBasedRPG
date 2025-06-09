cmake_minimum_required(VERSION 3.29.0-rc3)
project(
    TestWebGPU
    VERSION 0.1.0
    LANGUAGES CXX C
)

# Manually list all source files
set(SOURCES
    "src/main.cpp"
    "src/gpu/WebGPUContext.cpp"
    "src/gpu/WebGPUDevice.cpp"
    "src/gpu/WebGPUAdapter.cpp"
)

# Manually list all header files
set(HEADERS
    "src/gpu/WebGPUContext.h"
    "src/gpu/WebGPUDevice.h"
    "src/gpu/WebGPUAdapter.h"
)

# Combine source and header files
set(ALL_FILES ${SOURCES} ${HEADERS})

# Group files in Visual Studio according to folder structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${ALL_FILES})

# Create the executable with explicit source files
add_executable(App ${ALL_FILES})
target_link_libraries(App PRIVATE webgpu)

# Set C++ standard and warnings
set_target_properties(App PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

# Compiler-specific warning settings
if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
endif()

# Add webgpu subdirectory
add_subdirectory(webgpu)
target_copy_webgpu_binaries(App)

# Add include directories
target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)