TARGET=slingshot
PARSER=parser.cpp
LEXER=lexer.cpp
SRC=$(PARSER) $(LEXER)  main.cpp ast.cpp aux.cpp
OBJs=$(SRC: .cpp=.o)
.PHONY: clean run

$(TARGET): $(OBJs)
	g++ -std=c++17 -g -Wall -o $@ $(OBJs)

$(LEXER): lexer.l
	flex -o $@ $^

$(PARSER): grammar.y
	bison -rall --defines=token.h -o $@ $<

%.o: %.cpp %.h token.h
	g++ -std=c++17 -g -Wall -c -o  $@ $<

build: clean
	clear
	make -j

clean:
	rm -f $(PARSER)
	rm -f $(LEXER)
	rm -f $(TARGET)
	rm -f *.o
	rm -f token.h
	rm -f parser.output