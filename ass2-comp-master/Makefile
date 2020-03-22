comp: lex.yy.c lua.tab.o main.cc
	g++ --std=c++11 -g -ocomp lua.tab.o lex.yy.c main.cc asm.cc
lua.tab.o: lua.tab.cc
	g++ --std=c++11 -g -c lua.tab.cc
lua.tab.cc: lua.yy
	bison -r all lua.yy
lex.yy.c: lua.ll lua.tab.cc
	flex lua.ll
clean: 
	rm -rf lua.tab.* lex.yy.c* comp ./comp.dSYM *.output stack.hh cfg.dot* target.cc target