# Makefile for Basler Pylon sample program
.PHONY			: all clean install

# The program to build
NAME			:= errortiff


# Build tools and flags
CXX				?= g++

LD         := $(CXX)
CPPFLAGS   := $(shell pkg-config zbar --cflags) $(shell pkg-config opencv --cflags)
CXXFLAGS   := -std=c++11 #e.g., CXXFLAGS=-g -O0 for debugging
LDFLAGS    :=
LDLIBS     := $(shell pkg-config zbar --cflags --libs) $(shell pkg-config opencv --libs) -lboost_system -lboost_regex -lboost_filesystem

# Rules for building
all				: $(NAME)

$(NAME)			: $(NAME).o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(NAME).o: $(NAME).cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean			:
	$(RM) $(NAME).o $(NAME)


install		: $(NAME)
	echo "not implemented"
