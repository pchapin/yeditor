#
# Makefile for the Check project.
#

CXX=g++
CXXFLAGS=-Wall -std=c++11 -c -O -I.. -I../../../Spica/Cpp -I../../scr
LINK=g++
LINKFLAGS=-lncurses
SOURCES=check.cpp check_EditBuffer.cpp check_EditList.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTSTESTED=../EditBuffer.o
EXECUTABLE=check
LIBSPICA=../../../Spica/Cpp/libSpicaCpp.a
LIBSCR=../../scr/libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR)
	$(LINK) $(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Wed Jan 22 15:02:45 2020


check.o:	check.cpp check.hpp ../../../Spica/Cpp/UnitTestManager.hpp 

check_EditBuffer.o:	check_EditBuffer.cpp ../EditBuffer.hpp ../../../Spica/Cpp/UnitTestManager.hpp 

check_EditList.o:	check_EditList.cpp ../EditList.hpp ../mylist.hpp ../../../Spica/Cpp/UnitTestManager.hpp \
	

# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
