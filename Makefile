headers = shared.h   game_board.h game_manager.h search.h main.h
imp_files = main.cpp  game_manager.cpp game_board.cpp search.cpp main.cpp
Submit.cc: $(headers) $(imp_files)
    cat S.h $(headers) $(imp_files) > Submit.cc
