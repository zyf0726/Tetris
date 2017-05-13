cmake_minimum_required(VERSION 3.6)
project(Tetris)
set(CMAKE_CXX_STANDARD 11)

add_executable(main game_board.cpp main.cpp search.cpp game_manager.cpp
    random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp)
add_executable(ga_experiment ga_experiment.cpp)
add_executable(ga_summary ga_summary.cpp)
target_link_libraries(ga_summary pthread)
add_executable(genetic_algorithm genetic_algorithm.cpp)
