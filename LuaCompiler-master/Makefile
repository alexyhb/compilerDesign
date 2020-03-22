parser: lex.yy.c lua.tab.o main.cc
	g++ --std=c++11 -o int lua.tab.o lex.yy.c main.cc
lua.tab.o: lua.tab.cc
	g++ --std=c++11 -c lua.tab.cc
lua.tab.cc: lua.yy
	bison -r all lua.yy
lex.yy.c: lua.ll lua.tab.cc
	flex lua.ll
clean: 
	rm lua.tab.* lex.yy.c* int *.output stack.hh