# Makefile for Basler Pylon sample program
.PHONY			: all clean install

# The program to build
NAME			:= grab-barcode

# Installation directories for GenICam and Pylon
PYLON_ROOT		?= /opt/pylon5
GENICAM_ROOT	?= $(PYLON_ROOT)/genicam

# Build tools and flags
CXX				?= g++
#LD				:= $(CXX)
#CPPFLAGS		:= -I$(GENICAM_ROOT)/library/CPP/include \
#				   -I$(PYLON_ROOT)/include -DUSE_GIGE
#CXXFLAGS		:= #e.g., CXXFLAGS=-g -O0 for debugging
#LDFLAGS			:= -L$(PYLON_ROOT)/lib64 \
#				   -L$(GENICAM_ROOT)/bin/Linux64_x64 \
#				   -L$(GENICAM_ROOT)/bin/Linux64_x64/GenApi/Generic \
#				   -Wl,-E
#LIBS			:= -lpylonbase -lpylonutility -lGenApi_gcc40_v2_3 -lGCBase_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 -lXerces-C_gcc40_v2_7_1 -llog4cpp_gcc40_v2_3 -lpthread
#LIBS			:= -lpylonbase -lpylonutility -lGenApi_gcc40_v2_3 -lGCBase_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 -lXerces-C_gcc40_v2_7_1 -llog4cpp_gcc40_v2_3 -lpthread

LD         := $(CXX)
CPPFLAGS   := $(shell $(PYLON_ROOT)/bin/pylon-config --cflags) $(shell pkg-config opencv --cflags) -I/usr/local/include
CXXFLAGS   := #e.g., CXXFLAGS=-g -O0 for debugging
LDFLAGS    := $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)
LDLIBS     := $(shell $(PYLON_ROOT)/bin/pylon-config --libs) $(shell pkg-config opencv --libs) -lzbar

# Rules for building
all				: $(NAME)

$(NAME)			: $(NAME).o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(NAME).o: $(NAME).cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

clean			:
	$(RM) $(NAME).o $(NAME)


install		: $(NAME)
