#
# Makefile for the Check project.
#

CXX=g++
CXXFLAGS=-Wall -std=c++11 -c -O -I.. -I../../scr
LINK=g++
LINKFLAGS=-lncurses
SOURCES=check.cpp UnitTestManager.cpp check_EditBuffer.cpp check_EditList.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTSTESTED=../EditBuffer.o
EXECUTABLE=check
LIBSCR=../../scr/libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(OBJECTSTESTED) $(LIBSCR)
	$(LINK) $(OBJECTS) $(OBJECTSTESTED) $(LIBSCR) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Wed Jan 22 20:38:34 2020


check.o:	check.cpp check.hpp UnitTestManager.hpp 

UnitTestManager.o:	UnitTestManager.cpp UnitTestManager.hpp 

check_EditBuffer.o:	check_EditBuffer.cpp ../EditBuffer.hpp UnitTestManager.hpp 

check_EditList.o:	check_EditList.cpp ../EditList.hpp ../mylist.hpp UnitTestManager.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
