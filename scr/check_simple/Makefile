#
# Makefile for the CheckSimple project.
#

CXX=g++
CXXFLAGS=-std=c++11 -c -O -I..
LINK=g++
LINKFLAGS=-lncurses
SOURCES=basic_test.cpp \
	check.cpp      \
	key_test.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=check_simple
LIBSCR=../libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS) $(LIBSCR)
	$(LINK) -o $@ $(OBJECTS) $(LIBSCR) $(LINKFLAGS) 

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:47:39 2013


basic_test.o:	basic_test.cpp check.hpp ../scr.hpp 

check.o:	check.cpp ../scr.hpp check.hpp 

key_test.o:	key_test.cpp check.hpp ../scr.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
