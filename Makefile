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
INC=$(wildcard include/*.h)

.PHONY: clean

release: CXXFLAGS += -O2
release: ot
	strip ot
	upx --brute ot

debug: CXXFLAGS += -g
debug: ot

ot: $(OBJ)
	$(CXX) -o ot $(OBJ) $(LDLIBS)

cleanup:
	perl -i -pe 's/\s+\n/\n/' ${SRC} $(INC)
	perl -i -pe 's/[\t ]+$$//g' ${SRC} $(INC)

cleanup2:
	ls ${SRC} $(INC) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

clean:
	$(RM) ot $(OBJ) $(DEP) 

-include $(DEP)
