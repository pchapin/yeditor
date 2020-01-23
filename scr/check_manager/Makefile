#
# Makefile for the CheckManager project.
#

CXX=g++
CXXFLAGS=-std=c++11 -c -O -I..
LINK=g++
LINKFLAGS=-lncurses
SOURCES=check.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=check_manager
LIBSCR=../libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(LIBSCR)
	$(LINK) -o $@ $(OBJECTS) $(LIBSCR) $(LINKFLAGS)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:42:33 2013


check.o:	check.cpp ../CommandWindow.hpp ../ImageBuffer.hpp ../scr.hpp ../Manager.hpp ../Window.hpp \
	../debug.hpp ../TextWindow.hpp ../ListWindow.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
