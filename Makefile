CXXFLAGS=-I./include -Wall -Wextra -MMD -MP -std=c++14
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CXXFLAGS += -I/usr/local/include
endif

LDLIBS=-lm

ifeq ($(UNAME_S),Darwin)
	LDLIBS += -L/usr/local/lib -lboost_filesystem -lboost_system
endif

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
	$(CXX) -o ot $(OBJ) $(LDLIBS)
	
clean:
	$(RM) ot $(OBJ) $(DEP) 

-include $(DEP)
