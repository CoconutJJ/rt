cmake_minimum_required(VERSION 3.30)

project(rt)

include_directories(include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

execute_process(COMMAND bash -c "xxd -i help.txt > include/usage.hpp")

add_library(utils STATIC "utils.cpp")
add_library(vec3 STATIC "vec3.cpp")
add_library(ray STATIC "ray.cpp")
add_library(mat3 STATIC "mat3.cpp")
add_library(kdtree STATIC "kdtree.cpp")
add_library(world STATIC "world/camera.cpp" "world/world.cpp" "world/hitrecord.cpp")
add_library(texture STATIC "texture/image_texture.cpp" "texture/solid_texture.cpp" "texture/texture.cpp" "texture/checkerboard.cpp")
add_library(object STATIC "object/object.cpp" "object/smooth_object.cpp" "object/plane.cpp" "object/quad.cpp" "object/sphere.cpp" "object/triangle.cpp" "object/mesh.cpp")
add_library(material STATIC "material/material.cpp" "material/phong.cpp" "material/lambertian.cpp" "material/dielectric.cpp")
add_library(light STATIC "light/light.cpp" "light/point_light.cpp" "light/quad_light.cpp")
add_library(io STATIC "io/obj.cpp")
add_library(pt STATIC "pt/BRDFRead.cpp")
add_library(catch2 "tests/catch_amalgamated.cpp")    

add_executable(rt "main.cpp")
add_executable(height_map_conv "scripts/bin/height_map.cpp" "utils.cpp")
add_executable(obj_file "scripts/bin/obj_file.cpp" "utils.cpp")

target_link_libraries(rt vec3 ray mat3 world texture object material light io pt kdtree utils)
target_link_libraries(height_map_conv vec3 ray mat3)
target_link_libraries(obj_file vec3 io ray mat3)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test_KDTree "tests/kdtree/test_kdtree.cpp")
add_executable(test_utils "tests/utils/test_utils.cpp")
target_link_libraries(test_KDTree kdtree vec3 utils ray mat3 catch2)
target_link_libraries(test_utils vec3 utils ray mat3 catch2)

add_custom_target(tests DEPENDS test_KDTree test_utils)

include(Catch)
catch_discover_tests(test_KDTree WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
