parser: lex.yy.c binary.tab.o main.cc
		g++ -g -oparser binary.tab.o lex.yy.c main.cc
binary.tab.o: binary.tab.cc
		g++ -g -c binary.tab.cc
binary.tab.cc: binary.yy
		bison binary.yy
lex.yy.c: binary.ll binary.tab.cc
		flex binary.ll
clean: 
		rm -f binary.tab.* lex.yy.c* parser