cmake_minimum_required(VERSION 3.0.0)   # Selects the minimum version of CMake required to run this file
project(GFX-LAB VERSION 0.1.0)          # Here we select the project name and version

# Here we select C++17 with all the standards required and all compiler-specific extensions disabled
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")


# These are the options we select for building GLFW as a library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Add variables to use High Performance Graphics Card if available
add_subdirectory(vendor/glfw)                       # Build the GLFW project to use later as a library

set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
# set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
# add_subdirectory(vendor/bullet3-3.25)              
add_subdirectory(vendor/bullet3-3.25)              
# add_subdirectory(vendor/bullet-2.81-rev2613/src) 
# add_subdirectory(vendor/bullet/BulletCollision) 
# add_subdirectory(vendor/bullet/LinearMath) 
# add_subdirectory(vendor/bullet/BulletCommon) 

# A variable with all the source files of GLAD
set(GLAD_SOURCE vendor/glad/src/gl.c)
# A variables with all the source files of Dear ImGui
set(IMGUI_SOURCES
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_impl/imgui_impl_glfw.cpp
        vendor/imgui/imgui_impl/imgui_impl_opengl3.cpp
)

# A variable with all source files of ImGuizmo
set(IMGUIZMO_SOURCES
        vendor/ImGuizmo/ImCurveEdit.cpp
        vendor/ImGuizmo/ImGradient.cpp
        vendor/ImGuizmo/ImGuizmo.cpp
        vendor/ImGuizmo/ImSequencer.cpp
)
# ReactPhysics3D
#add_subdirectory(vendor/reactphysics3D)

# FreeType
add_subdirectory(vendor/utils/freetype-2.13.2)

# Combine all vendor source files together into a single variable
set(VENDOR_SOURCES ${GLAD_SOURCE} ${IMGUI_SOURCES} ${IMGUIZMO_SOURCES})

# A variable with all our source files that are common between executable targets (examples)
set(COMMON_SOURCES
        source/common/application.hpp
        source/common/application.cpp
        source/common/input/keyboard.hpp
        source/common/input/mouse.hpp

        source/common/light/light-utils.hpp

        source/common/asset-loader.cpp
        source/common/asset-loader.hpp
        source/common/deserialize-utils.hpp
        source/common/imgui-utils.hpp

        source/common/shader/shader.hpp
        source/common/shader/shader.cpp

        source/common/mesh/vertex.hpp
        source/common/mesh/mesh.hpp
        source/common/mesh/mesh.cpp
        source/common/mesh/mesh-utils.hpp
        source/common/mesh/mesh-utils.cpp

        source/common/texture/sampler.hpp
        source/common/texture/sampler.cpp
        source/common/texture/texture2d.hpp
        source/common/texture/texture3d.hpp
        source/common/texture/texture-utils.hpp
        source/common/texture/texture-utils.cpp
        source/common/texture/screenshot.hpp
        source/common/texture/screenshot.cpp

        source/common/physics/physics-utils.hpp

        source/common/material/pipeline-state.hpp
        source/common/material/pipeline-state.cpp
        source/common/material/material.hpp
        source/common/material/material.cpp

        source/common/ecs/component.hpp
        source/common/ecs/transform.hpp
        source/common/ecs/transform.cpp
        source/common/ecs/entity.hpp
        source/common/ecs/entity.cpp
        source/common/ecs/world.hpp
        source/common/ecs/world.cpp


        source/miniaudio_wrapper.hpp
        source/miniaudio_wrapper.cpp

        # source/states/miniaudio_wrapper.cpp


)


# A variable with all components
set(COMMON_COMPONENTS_SOURCES
        source/common/components/camera.hpp
        source/common/components/camera.cpp
        source/common/components/mesh-renderer.hpp
        source/common/components/mesh-renderer.cpp
        source/common/components/light-component.hpp
        source/common/components/light-component.cpp
        source/common/components/free-camera-controller.hpp
        source/common/components/ground-or-stairs.hpp
        source/common/components/fps-camera-controller.hpp
        source/common/components/camera-controller.hpp
        source/common/components/camera-controller.cpp
        source/common/components/movement.hpp
        source/common/components/movement.cpp
        source/common/components/component-deserializer.hpp
        #        source/common/components/player.cpp
        #        source/common/components/player.hpp
        source/common/components/rigid-body.cpp
        source/common/components/rigid-body.hpp
        source/common/components/trigger.cpp
        source/common/components/trigger.hpp
        source/common/components/pickable.cpp
        source/common/components/pickable.hpp
        source/common/components/knob-component.cpp
        source/common/components/knob-component.hpp

)

# A variable with all systems
set(COMMON_SYSTEMS_SOURCES
        source/common/systems/text-renderer.hpp
        source/common/systems/text-renderer.cpp
        source/common/systems/forward-renderer.hpp
        source/common/systems/forward-renderer.cpp
        source/common/systems/camera-controller.hpp
        source/common/systems/free-camera-controller.hpp
        source/common/systems/fps-camera-controller.hpp
        source/common/systems/movement.hpp
        source/common/systems/light.hpp

        source/miniaudio_wrapper.hpp
        source/miniaudio_wrapper.cpp
        # source/states/miniaudio_wrapper.cpp
        source/common/systems/physics.hpp
        source/common/systems/locked-away.cpp
        source/common/systems/locked-away.hpp
        source/common/systems/physics.cpp
        source/common/systems/picking.cpp
        source/common/systems/picking.hpp
        source/common/systems/drawer-opener.cpp
        source/common/systems/drawer-opener.hpp
        #        source/common/systems/movement.cpp
)


# Define the directories in which to search for the included headers
include_directories(
        source/common
        vendor/glfw/include
        vendor/glad/include
        vendor/glm
        vendor/imgui
        vendor/utils
        vendor/bullet3-3.25/src
        vendor/utils/freetype-2.13.2/include
#        vendor/reactphysics3D/include
        vendor/ImGuizmo
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

set(STATES_SOURCES
        source/states/play-state.hpp
        source/states/menu-state.hpp
        source/states/lose-state.hpp
        source/states/win-state.hpp

)


# For each example, we add an executable target
# Each target compiles one example source file and the common & vendor source files
# Then we link GLFW with each target
add_executable(GAME_APPLICATION source/main.cpp ${STATES_SOURCES} ${COMMON_SOURCES} ${VENDOR_SOURCES} ${COMMON_SYSTEMS_SOURCES}
        ${COMMON_COMPONENTS_SOURCES} vendor/bullet3-3.25/src)
target_link_libraries(GAME_APPLICATION glfw freetype BulletDynamics BulletCollision LinearMath)

#target_link_libraries(GAME_APPLICATION reactphysics3d)