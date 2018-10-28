cmake_minimum_required(VERSION 3.8)
project(Peter_Shirley_Project_Code)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp vec3.h ray.h hitable.h sphere.h cmake-build-debug/picture.png hitable_list.h stb_image_write.h stb_image.h camera.h material.h aabb.h bvh.h texture.h perlin.h surface_texture.h aarect.h box.h constant_medium.h)
add_executable(Peter_Shirley_Project_Code ${SOURCE_FILES})