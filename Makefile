
parser: lex.yy.c grammar.tab.o Nodes.o Environment.o main.cc
	g++ -g -oparser grammar.tab.o Nodes.o Environment.o lex.yy.c main.cc
grammar.tab.o: grammar.tab.cc
	g++ -g -c grammar.tab.cc

Nodes.o: Nodes.cc Nodes.h
	g++ -g -c Nodes.cc

Environment.o: Environment.cc Environment.h
	g++ -g -c Environment.cc

grammar.tab.cc: grammar.yy
	bison grammar.yy -v
lex.yy.c: lexer.ll grammar.tab.cc
	flex lexer.ll
clean:
	rm -f grammar.tab.* lex.yy.c* parser tree.pdf
