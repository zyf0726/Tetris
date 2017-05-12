cmake_minimum_required(VERSION 3.6)
project(Tetris)
set(CMAKE_CXX_STANDARD 11)

add_executable(main game_board.cpp main.cpp search.cpp game_manager.cpp)
#add_executable(Sample sample.cpp)
