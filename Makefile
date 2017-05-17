headers = shared.h options.h tetromino.h board.h  random.h feature_functions.h feature_helpers.h genotype.h phenotype.h game_board.h game_manager.h search.h main.h 
imp_files =  board.cpp feature_functions.cpp feature_helpers.cpp game_board.cpp game_manager.cpp genotype.cpp main.cpp options.cpp phenotype.cpp random.cpp search.cpp  tetromino.cpp
Submit.cc: $(headers) $(imp_files)
	cat S.h $(headers) $(imp_files) > Submit.cc
	cp  -f Submit.cc cmake-build-debug/
	g++ -g -O2 -D_BOTZONE_ONLINE cmake-build-debug/Submit.cc -o Submit -std=c++11 -Wall -Wextra --pedantic
clean:
	rm -f Submit.cc Submit
