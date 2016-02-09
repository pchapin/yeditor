#
# Makefile for the Spica C++ library.
#

CXX=g++
CXXFLAGS=-std=c++11 -Wall -c -O
LINK=ar
LINKFLAGS=rc
SOURCES=base64.cpp           \
	BitFile.cpp          \
	config.cpp           \
	crc.cpp              \
	Date.cpp             \
	get_switch.cpp       \
	str.cpp              \
	string_utilities.cpp \
	synchronize.cpp      \
	Timer.cpp            \
	UnitTestManager.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libSpicaCpp.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(LIBRARY):	$(OBJECTS)
	$(LINK) $(LINKFLAGS) -o $@ $(OBJECTS)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sun Jul 14 15:50:43 2013


base64.o:	base64.cpp base64.hpp 

BitFile.o:	BitFile.cpp BitFile.hpp 

config.o:	config.cpp config.hpp 

crc.o:	crc.cpp crc.hpp 

Date.o:	Date.cpp Date.hpp 

get_switch.o:	get_switch.cpp get_switch.hpp 

str.o:	str.cpp environ.hpp str.hpp synchronize.hpp 

string_utilities.o:	string_utilities.cpp string_utilities.hpp 

Timer.o:	Timer.cpp Timer.hpp environ.hpp 

UnitTestManager.o:	UnitTestManager.cpp UnitTestManager.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) *.s *.ll *~
