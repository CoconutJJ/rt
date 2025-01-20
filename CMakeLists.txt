cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")

# set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/gcc/bin/g++-14")
# set(CMAKE_C_COMPILER "/opt/homebrew/opt/gcc/bin/gcc-14")

project(rt CXX C)

include_directories(include)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -march=native -mtune=native -fuse-ld=lld")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -flto")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

execute_process(COMMAND bash -c "xxd -i help.txt > include/usage.hpp")

add_library(utils STATIC "src/utils.cpp")
add_library(ds STATIC "src/ds/vec3.cpp" "src/ds/ray.cpp" "src/ds/mat3.cpp" "src/ds/kdtree.cpp") 
add_library(world STATIC "src/world/camera.cpp" "src/world/world.cpp" "src/world/hitrecord.cpp")
add_library(texture STATIC "src/texture/image_texture.cpp" "src/texture/solid_texture.cpp" "src/texture/texture.cpp" "src/texture/checkerboard.cpp")
add_library(object STATIC "src/object/object.cpp" "src/object/smooth_object.cpp" "src/object/plane.cpp" "src/object/quad.cpp" "src/object/sphere.cpp" "src/object/triangle.cpp" "src/object/mesh.cpp")
add_library(material STATIC "src/material/material.cpp" "src/material/phong.cpp" "src/material/lambertian.cpp" "src/material/dielectric.cpp" "src/material/metal.cpp" "src/material/BRDF.cpp")
add_library(light STATIC "src/light/light.cpp" "src/light/point_light.cpp" "src/light/quad_light.cpp")
add_library(io STATIC "src/io/obj.cpp")
add_library(catch2 STATIC "src/tests/catch_amalgamated.cpp")    
add_library(rply STATIC "src/rply.c")

add_executable(rt "rt.cpp")
add_executable(height_map_conv "src/scripts/bin/height_map.cpp" "src/utils.cpp")
add_executable(obj_file "src/scripts/bin/obj_file.cpp" "src/utils.cpp")

target_link_libraries(rt ds world texture object material light io utils rply)
target_link_libraries(height_map_conv ds)
target_link_libraries(obj_file ds io)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test_KDTree "src/tests/kdtree/test_kdtree.cpp")
add_executable(test_utils "src/tests/utils/test_utils.cpp")
add_executable(test_mesh "src/tests/object/test_mesh.cpp")

target_link_libraries(test_KDTree utils ds catch2)
target_link_libraries(test_utils utils ds catch2)
target_link_libraries(test_mesh utils ds object world pt catch2)
add_custom_target(tests DEPENDS test_KDTree test_utils test_mesh)

include(Catch)
catch_discover_tests(test_KDTree WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# -fprofile-instr-generate -fcoverage-mapping