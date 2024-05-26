cmake_minimum_required(VERSION 3.20)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "utility")

# language settings
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

project(CSGO LANGUAGES C CXX VERSION 0.0)

# build in source in not recommended
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(WARNING "The binary directory of CMake should not \
                     be the same as source directory")
endif()

# build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# disable max/min definition in windows
if(WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory(./external/glfw)
set_target_properties(glfw PROPERTIES FOLDER "lib/glfw")
set_target_properties(uninstall PROPERTIES FOLDER "lib/glfw")
set_target_properties(update_mappings PROPERTIES FOLDER "lib/glfw")

add_subdirectory(./external/glad)
set_target_properties(glad PROPERTIES FOLDER "lib")

add_subdirectory(./external/glm)
set_target_properties(glm PROPERTIES FOLDER "lib/glm")

add_subdirectory(./external/tinyobjloader)
set_target_properties(tinyobjloader PROPERTIES FOLDER "lib")

add_subdirectory(./external/imgui)
set_target_properties(imgui PROPERTIES FOLDER "lib")

add_subdirectory(./external/stb)
set_target_properties(stb PROPERTIES FOLDER "lib")

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_HEADER_ONLY OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
add_subdirectory(./external/tinygltf)
set_target_properties(tinygltf PROPERTIES FOLDER "lib")

file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB BASE_SOURCE_FILES "base/src/*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/base/include)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${BASE_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "app")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyobjloader)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE stb)