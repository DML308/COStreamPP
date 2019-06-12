PROGRAM := test
SOURCES := $(wildcard ./*.cpp)
SOURCES += $(wildcard ./src/*.cpp)
OBJS    := $(patsubst %.cpp,%.o,$(SOURCES))
CC      := g++
CFLAGS  := -ggdb -Wall
INCLUDE := -I . -I/usr/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc
LIB     := -lpthread -lOpenCL


.PHONY: clean install

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(LIB) 
%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE)


clean:
	rm $(OBJS) $(PROGRAM) -f


install: $(PROGRAM)
	cp $(PROGRAM) ./bin/
