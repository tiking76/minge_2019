cmake_minimum_required(VERSION 3.14)
project(2019_minge)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(2019_minge main.cpp SDL2_DxLib.cpp SDL2_DxLib.h )

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(OpenGL)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(2019_minge ${OPENGL_LIBRARY})
target_link_libraries(2019_minge ${SDL2_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_TTF_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_MIXER_LIBRARIES})