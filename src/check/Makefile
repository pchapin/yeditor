#
# Makefile for the Check project.
#

CXX=g++
CXXFLAGS=-Wall -std=c++11 -c -O -I.. -I../../Spica -I../../Scr
LINK=g++
LINKFLAGS=-lncurses
SOURCES=check.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTSTESTED=../EditBuffer.o
EXECUTABLE=check
LIBSPICA=../../Spica/libSpicaCpp.a
LIBSCR=../../Scr/libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR)
	$(LINK) $(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:55:04 2013


check.o:	check.cpp ../EditBuffer.hpp ../../Spica/UnitTestManager.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
