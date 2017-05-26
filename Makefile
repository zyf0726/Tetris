headers = shared.h options.h tetromino.h board.h  random.h feature_functions.h feature_helpers.h genotype.h phenotype.h game_board.h game_manager.h search.h main.h  selection.h population.h 
imp_files =  board.cpp feature_functions.cpp feature_helpers.cpp game_board.cpp game_manager.cpp genotype.cpp options.cpp phenotype.cpp random.cpp search.cpp  tetromino.cpp population.cpp selection.cpp
ALL: Submit Train
Submit: $(headers) $(imp_files) main.cpp
	cat S.h $(headers) $(imp_files) main.cpp > Submit.cc
	cat S_train.h $(headers) $(imp_files) tetris.cpp > Train.cc
	cp  -f Submit.cc cmake-build-debug/
	clang++ -g -O2 -D_BOTZONE_ONLINE cmake-build-debug/Submit.cc -o Submit -std=c++11 -Wall -Wextra 
Train:  
	cat S_train.h $(headers) $(imp_files) tetris.cpp > Train.cc
	cp  -f Train.cc cmake-build-debug/
	mpic++ -g -Ofast -funroll-loops -march=native  cmake-build-debug/Train.cc -o Train -std=c++11 -Wall -Wextra
clean:
	rm -f Submit.cc Submit Train.cc Train
