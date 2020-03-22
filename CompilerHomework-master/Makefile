parser: lex.yy.c task3.tab.o main.cc
	g++ -std=c++11 -g -oparser task3.tab.o lex.yy.c main.cc
task3.tab.o: task3.tab.cc
	g++ -std=c++11 -g -c task3.tab.cc
task3.tab.cc: task3.yy
	bison -r all task3.yy
lex.yy.c: task3.ll task3.tab.cc
	flex task3.ll
clean:
	rm -rf task3.tab.* lex.yy.c* stack.hh parser parser.dSYM *.output
