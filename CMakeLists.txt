cmake_minimum_required(VERSION 3.5)
project(rt)

include_directories(include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(vec3 STATIC "vec3.cpp")
add_library(ray STATIC "ray.cpp")
add_library(mat3 STATIC "mat3.cpp")
add_library(world STATIC "world/camera.cpp" "world/world.cpp" "world/hitrecord.cpp")
add_library(texture STATIC "texture/image_texture.cpp" "texture/solid_texture.cpp" "texture/texture.cpp" "texture/checkerboard.cpp")
add_library(object STATIC "object/object.cpp" "object/plane.cpp" "object/quad.cpp" "object/sphere.cpp")
add_library(material STATIC "material/material.cpp" "material/phong.cpp")
add_library(light STATIC "light/light.cpp" "light/point_light.cpp" "light/quad_light.cpp")

add_executable(rt "main.cpp" "utils.cpp")
add_executable(height_map_conv "scripts/bin/height_map.cpp" "utils.cpp")


target_link_libraries(rt vec3 ray mat3 world texture object material light)
target_link_libraries(height_map_conv vec3)





