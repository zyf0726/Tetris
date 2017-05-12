headers = shared.h	 game_board.h game_manager.h search.h main.h
imp_files = main.cpp  game_manager.cpp game_board.cpp search.cpp 
Submit.cc: $(headers) $(imp_files)
	cat S.h $(headers) $(imp_files) > Submit.cc
	cp  -f Submit.cc cmake-build-debug/
	c++ cmake-build-debug/Submit.cc -o Submit -std=c++11
clean:
	rm -f Submit.cc Submit
