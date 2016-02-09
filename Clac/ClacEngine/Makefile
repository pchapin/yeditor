#
# Makefile for the clac engine library.
#

CXX=g++
CXXFLAGS=-std=c++11 -c -O -I../ClacEntity
LINK=ar
LINKFLAGS=rc
SOURCES=actions.cpp    \
	cstack.cpp     \
	get.cpp        \
	global.cpp     \
	words.cpp      \
	WordStream.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libClacEngine.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(LIBRARY):	$(OBJECTS)
	$(LINK) $(LINKFLAGS) -o $@ $(OBJECTS)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Sep 28 08:19:29 2013


actions.o:	actions.cpp ../ClacEntity/BinaryEntity.hpp ../ClacEntity/Entity.hpp ../ClacEntity/ComplexEntity.hpp \
	../ClacEntity/FloatEntity.hpp ../ClacEntity/IntegerEntity.hpp ../ClacEntity/VeryLong.hpp \
	../ClacEntity/ListEntity.hpp ../ClacEntity/MatrixEntity.hpp ../ClacEntity/RationalEntity.hpp \
	../ClacEntity/Rational.hpp ../ClacEntity/StringEntity.hpp actions.hpp cstack.hpp global.hpp \
	WordStream.hpp ../ClacEntity/support.hpp 

cstack.o:	cstack.cpp cstack.hpp ../ClacEntity/Entity.hpp ../ClacEntity/VeryLong.hpp ../ClacEntity/support.hpp \
	

get.o:	get.cpp ../ClacEntity/Rational.hpp ../ClacEntity/BinaryEntity.hpp ../ClacEntity/Entity.hpp \
	../ClacEntity/ComplexEntity.hpp ../ClacEntity/FloatEntity.hpp ../ClacEntity/IntegerEntity.hpp \
	../ClacEntity/VeryLong.hpp ../ClacEntity/LabeledEntity.hpp ../ClacEntity/ListEntity.hpp \
	../ClacEntity/MatrixEntity.hpp ../ClacEntity/RationalEntity.hpp ../ClacEntity/StringEntity.hpp \
	../ClacEntity/support.hpp get.hpp WordStream.hpp global.hpp cstack.hpp words.hpp 

global.o:	global.cpp global.hpp ../ClacEntity/VeryLong.hpp cstack.hpp ../ClacEntity/Entity.hpp \
	WordStream.hpp 

words.o:	words.cpp words.hpp 

WordStream.o:	WordStream.cpp WordStream.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) *.s *.ll *~
