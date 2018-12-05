CXX		= g++ -std=c++11 -w
YACC    = bison -y
LEX     = flex
CPPFLAGS  = -I include/FrontEnd -I include/BackEnd

TAB = '	'
VPATH = .:bin:config
PROGRAM := a
SRCDIRS := src src/FrontEnd src/BackEnd
OUTDIRS := bin
SRCEXTS := .cpp # SRC 目录里相关的文件后缀

SHELL := /bin/sh
SOURCES := $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.cpp))
FILTEROUT := src/FrontEnd/parser.cpp src/FrontEnd/lexer.cpp
OBJS := parser.o lexer.o $(foreach x,$(SRCEXTS),$(patsubst %$(x),%.o,$(notdir $(filter-out $(FILTEROUT),$(SOURCES)))))

# Rules for producing the executable.
#----------------------------------------------
all : $(PROGRAM)

parser.o: parser.y
	@echo SOURCES$(TAB)= ${SOURCES}
	@echo OBJS$(TAB)= ${OBJS}
	@echo
	@mkdir -p bin
	$(YACC) -v --defines=include/FrontEnd/token.h config/parser.y
	mv y.tab.c src/FrontEnd/parser.cpp
	mv y.output config/y.output
	$(CXX) $(CPPFLAGS) -DYYDEBUG -c -g src/FrontEnd/parser.cpp -o $(OUTDIRS)/parser.o
lexer.o:  lexer.l
	$(LEX) config/lexer.l
	mv lex.yy.c src/FrontEnd/lexer.cpp
	$(CXX) $(CPPFLAGS) -g -c src/FrontEnd/lexer.cpp -o bin/lexer.o

main.o : src/main.cpp
	$(CXX) -c $(CPPFLAGS) -g -o $(OUTDIRS)/$@ $<
%.o : src/FrontEnd/%.cpp
	$(CXX) -c $(CPPFLAGS) -g -o $(OUTDIRS)/$@ $<
%.o : src/BackEnd/%.cpp
	$(CXX) -c $(CPPFLAGS) -g -o $(OUTDIRS)/$@ $<

$(PROGRAM) : $(OBJS)
	$(CXX) -g -o $(PROGRAM) $(CPPFLAGS) $(foreach d,$(OBJS),$(addprefix $(OUTDIRS)/,$(d)))

clean :
	@$(RM) src/*/lexer.cpp include/*/token.h src/*/parser.cpp y.tab.[ch] config/*.output *.o  bin/* *.d *.out
count :
	make clean
	find . -maxdepth 2  -name "*[.l|.y|.c|.cc|.cpp|.h|Makefile]"  | xargs cat | wc -l
