cmake_minimum_required(VERSION 3.6)
project(Tetris)
set(CMAKE_CXX_STANDARD 11)

add_executable(main game_board.cpp main.cpp search.cpp game_manager.cpp
    random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp options.cpp)



add_executable(training  selection.cpp population.cpp phenotype.cpp genotype.cpp
random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp options.cpp tetris.cpp )

