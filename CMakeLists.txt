cmake_minimum_required(VERSION 3.6)
project(Tetris)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG -g3)
set(CMAKE_CXX_FLAGS_RELEASE -D_BOTZONE_ONLINE)
#set(CMAKE_BUILD_TYPE RELEASE)
add_executable(main shared.h game_board.cpp search.cpp main.cpp game_manager.cpp genotype.cpp phenotype.cpp
    random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp options.cpp)

add_executable(autogame_test shared.h game_board.cpp search.cpp autogame_test.cpp game_manager.cpp genotype.cpp phenotype.cpp
        random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp options.cpp)
find_package(MPI)

add_executable(training  shared.h selection.cpp population.cpp phenotype.cpp genotype.cpp game_manager.cpp search.cpp game_board.cpp
random.cpp board.cpp tetromino.cpp feature_functions.cpp feature_helpers.cpp options.cpp tetris.cpp )
target_include_directories(training PUBLIC ${MPI_INCLUDE_PATHS})
target_link_libraries(training ${MPI_LIBRARIES})

