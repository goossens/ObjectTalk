CXXFLAGS=-I./include -Wall -Wextra -MMD -MP -std=c++14
LDLIBS=-lm

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
DEP=$(SRC:.cpp=.d)

.PHONY: clean

release: CXXFLAGS += -O2
release: ot
	strip ot
	upx --brute ot

debug: CXXFLAGS += -g
debug: ot

ot: $(OBJ)
	$(CXX) -o ot $(OBJ)
	
clean:
	$(RM) ot $(OBJ) $(DEP) 

-include $(DEP)
