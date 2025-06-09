cmake_minimum_required(VERSION 3.29.0-rc3)
project(
    MUDENGINE
    VERSION 0.4.58
    LANGUAGES CXX C
)

# ========================================================================
#  Engine Project
# ========================================================================

include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

# List Engine source files
set(ENGINE_SOURCES
    "Engine/core/Core.cpp"
    "Engine/glfw/WindowHandler.cpp"
    "Engine/wgpu/buffer/UniformBuffers.cpp"
    "Engine/wgpu/buffer/VertexBuffer.cpp"
    "Engine/wgpu/buffer/IndexBuffer.cpp"
    "Engine/wgpu/pipelines/Quad2DPipeline.cpp"
    "Engine/wgpu/system/SurfaceHandler.cpp"
    "Engine/utilities/TextureImage.cpp"
)

# List Engine header files
set(ENGINE_HEADERS
    "Engine/core/Core.h"
    "Engine/glfw/WindowHandler.h"
    "Engine/wgpu/system/Adapter.h"
    "Engine/wgpu/system/Device.h"
    "Engine/wgpu/system/Queue.h"
    "Engine/wgpu/buffer/UniformBuffers.h"
    "Engine/wgpu/buffer/VertexBuffer.h"
    "Engine/wgpu/buffer/IndexBuffer.h"
    "Engine/utilities/Quad.h"
    "Engine/wgpu/pipelines/Quad2DPipeline.h"
    "Engine/wgpu/renderers/Quad2DRenderPass.h"
    "Engine/wgpu/system/SurfaceHandler.h"
    "Engine/core/Surface.h"
    "Engine/core/Window.h"
    "Engine/utilities/TextureImage.h"
    "Engine/utilities/stbi_image.h"
)

# Group all Engine files in Visual Studio under the "Engine" folder
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Engine PREFIX "Engine" 
    FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS}
)

# Create the Engine library
add_library(Engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Include Engine headers in the Engine library
target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
)

# Link external libraries to Engine
target_link_libraries(Engine PRIVATE webgpu glfw glfw3webgpu glm::glm)

# Set properties for the Engine library
set_target_properties(Engine PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

# ========================================================================
#  App Project
# ========================================================================
# List App source files
set(APP_SOURCES
    "App/main.cpp"
)

# List App header files
set(APP_HEADERS
    "App/Config.h"
)

# Group all App files in Visual Studio under the "App" folder
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/App PREFIX "App" 
    FILES ${APP_SOURCES} ${APP_HEADERS}
)

# Create the App executable
add_executable(App ${APP_SOURCES} ${APP_HEADERS})

# Link the Engine library and external libraries to App
target_link_libraries(App PRIVATE Engine webgpu glfw glfw3webgpu glm::glm)

# Include Engine headers in the App project
target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Engine)

# Set properties for the App executable
set_target_properties(App PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
)

# Set compile options based on the compiler
if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
endif()

# ==== External Libraries ====
add_subdirectory(glfw)
add_subdirectory(glfw3webgpu)
add_subdirectory(webgpu)

# Copy WebGPU binaries for App
target_copy_webgpu_binaries(App)
