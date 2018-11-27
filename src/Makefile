CXX		= g++ -std=c++11 -w
YACC    = bison -y
LEX     = flex
CPPFLAGS  = -I include/FrontEnd -I include/BackEnd

TAB = '	'
VPATH = .:bin:config
PROGRAM := a
SRCDIRS := src
OUTDIRS := bin
SRCEXTS := .cpp # SRC 目录里相关的文件后缀

SHELL := /bin/sh
SOURCES := $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
POBJ := parser.o lexer.o
OBJS := $(POBJ) $(foreach x,$(SRCEXTS),$(patsubst %$(x),%.o,$(notdir $(filter %$(x),$(SOURCES)))))

# Rules for producing the executable.
#----------------------------------------------
all : $(PROGRAM)

parser.o: parser.y
	@mkdir -p bin
	$(YACC) -v --defines=include/FrontEnd/token.h config/parser.y
	mv y.tab.c $(SRCDIRS)/FrontEnd/parser.cpp
	mv y.output config/y.output
	$(CXX) $(CPPFLAGS) -DYYDEBUG -c -g $(SRCDIRS)/FrontEnd/parser.cpp -o $(OUTDIRS)/parser.o
lexer.o:  lexer.l
	@echo SOURCES$(TAB)= ${SOURCES}
	@echo OBJS$(TAB)= ${OBJS}
	$(LEX) config/lexer.l
	mv lex.yy.c src/FrontEnd/lexer.cpp
	$(CXX) $(CPPFLAGS) -g -c src/FrontEnd/lexer.cpp -o bin/lexer.o


objs : $(OBJS)
%.o : $(SRCDIRS)/%.cpp
	$(CXX) -c $(CPPFLAGS) -g -o $(OUTDIRS)/$@ $<

$(PROGRAM) : $(OBJS)
	$(CXX) -g -o $(PROGRAM) $(CPPFLAGS) $(foreach d,$(OBJS),$(addprefix $(OUTDIRS)/,$(d)))


clean :
	@$(RM) src/*/lexer.c include/*/token.h src/*/parser.c y.tab.[ch] config/*.output *.o  bin/* *.d *.out
count :
	make clean
	find . -maxdepth 2  -name "*[.l|.y|.c|.cc|.cpp|.h|Makefile]"  | xargs cat | wc -l
