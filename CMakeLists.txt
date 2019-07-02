cmake_minimum_required(VERSION 3.14)
project(2019_minge)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(2019_minge main.cpp SDL2_DxLib.cpp SDL2_DxLib.h SceneMgr.cpp SceneMgr.h Menu.cpp Menu.h preferense.h Shoot.cpp Shoot.h CheckKey.cpp CheckKey.h Character.h Player.cpp Player.h Variable.h)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_library(OenGL OpenGL)
find_library(SDL2 SDL2)
find_library(SDL2_mixer SDL2_mixer)
find_library(SDL2_image SDL_image)
find_library(SDL2_ttf SDL_ttf)


include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(2019_minge ${OPENGL_LIBRARY})
target_link_libraries(2019_minge ${SDL2_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_TTF_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(2019_minge ${SDL2_MIXER_LIBRARIES})