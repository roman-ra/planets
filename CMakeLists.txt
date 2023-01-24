cmake_minimum_required(VERSION 3.18)

project(planets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

add_executable(planets 
    ext/stb_impl.c
    ext/tinyobjloader_impl.cpp
    src/glad.c

    src/ShaderProgram.cpp
    src/StaticMesh.cpp
    src/Material.cpp
    src/Texture2D.cpp
    src/ResourceManager.cpp


    src/SpatialObject.cpp
    src/StaticMeshInstance.cpp
    src/Scene.cpp

    src/Application.cpp
    src/Application_Platform.cpp 
    src/Application_InitScene.cpp 
    src/Main.cpp)

# Dependencies
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# ImGui
# =========================================================
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/ext/imgui)
file(GLOB IMGUI_GLOB
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imconfig.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_internal.h
    ${IMGUI_PATH}/imstb_rectpack.h
    ${IMGUI_PATH}/imstb_textedit.h
    ${IMGUI_PATH}/imstb_truetype.h
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp

    ${IMGUI_PATH}/backends/imgui_impl_glfw.h
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp)
add_library(imgui STATIC ${IMGUI_GLOB})
target_include_directories(imgui PUBLIC ${IMGUI_PATH})
# =========================================================

target_link_libraries(planets glm glfw fmt spdlog imgui)

target_include_directories(planets PUBLIC include)
target_include_directories(planets PUBLIC ext)