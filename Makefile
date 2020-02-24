FLAGS = -std=c++11 -g -Wall -Wpedantic -Werror


parser: lex.yy.c grammar.tab.o Nodes.o Environment.o main.cc
	g++ $(FLAGS) -oparser grammar.tab.o Nodes.o Environment.o lex.yy.c main.cc
grammar.tab.o: grammar.tab.cc
	g++ $(FLAGS) -c grammar.tab.cc

Nodes.o: Nodes.cc Nodes.h
	g++ $(FLAGS) -c Nodes.cc

Environment.o: Environment.cc Environment.h
	g++ $(FLAGS) -c Environment.cc

grammar.tab.cc: grammar.yy
	bison grammar.yy -v
lex.yy.c: lexer.ll grammar.tab.cc
	flex lexer.ll
clean:
	rm -f grammar.tab.* lex.yy.c* parser tree.pdf Environment.o grammar.output graph.dot *.o
