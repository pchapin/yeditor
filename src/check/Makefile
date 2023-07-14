#
# Makefile for the Check project.
#

CXX=g++
CXXFLAGS=-Wall -std=c++20 -c -O -I.. -I../Scr -I../SpicaCpp
LINK=g++
LINKFLAGS=-lncurses
SOURCES=check.cpp        \
	EditBuffer_tests.cpp \
	EditList_tests.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTSTESTED=../EditBuffer.o ../EditList.o
EXECUTABLE=check
LIBSCR=../Scr/libScr.a
LIBSPICACPP=../SpicaCpp/libSpicaCpp.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(OBJECTSTESTED) $(LIBSCR) $(LIBSPICACPP)
	$(LINK) $(OBJECTS) $(OBJECTSTESTED) $(LIBSCR) $(LIBSPICACPP) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Wed Jan 22 20:38:34 2020


check.o:	check.cpp check.hpp ../SpicaCpp/UnitTestManager.hpp 

check_EditBuffer.o:	check_EditBuffer.cpp ../EditBuffer.hpp ../SpicaCpp/UnitTestManager.hpp 

check_EditList.o:	check_EditList.cpp ../EditList.hpp ../mylist.hpp ../SpicaCpp/UnitTestManager.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
