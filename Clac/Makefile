#
# Makefile for the clac project.
#

CXX=g++
CXXFLAGS=-std=c++11 -c -O -I../Spica -I../Scr -IClacEntity -IClacEngine
LINK=g++
LINKFLAGS=-lncurses
SOURCES=clac.cpp              \
	ClacCommandWindow.cpp \
	DirectoryWindow.cpp   \
	record_f.cpp          \
	StackWindow.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=clac
LIBSPICA=../Spica/libSpicaCpp.a
LIBSCR=../Scr/libScr.a
LIBENTITY=ClacEntity/libClacEntity.a
LIBENGINE=ClacEngine/libClacEngine.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBSPICA) $(LIBSCR) $(LIBENGINE) $(LIBENTITY) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Sep 28 08:20:46 2013


clac.o:	clac.cpp ClacEntity/BinaryEntity.hpp ClacEntity/Entity.hpp ClacEntity/FloatEntity.hpp \
	ClacEntity/IntegerEntity.hpp ClacEntity/VeryLong.hpp ClacEntity/StringEntity.hpp ClacEntity/convert.hpp \
	ClacEntity/support.hpp ClacEngine/actions.hpp ClacEngine/cstack.hpp ClacEngine/get.hpp \
	ClacEngine/WordStream.hpp ClacEngine/global.hpp ClacEngine/words.hpp ../Scr/debug.hpp \
	../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp ../Scr/ImageBuffer.hpp ../Scr/MessageWindow.hpp \
	../Scr/Shadow.hpp ClacCommandWindow.hpp ../Scr/CommandWindow.hpp ../Scr/Manager.hpp DirectoryWindow.hpp \
	ClacEntity/DirectoryEntity.hpp StackWindow.hpp 

ClacCommandWindow.o:	ClacCommandWindow.cpp ClacCommandWindow.hpp ../Scr/CommandWindow.hpp ../Scr/ImageBuffer.hpp \
	../Scr/scr.hpp ../Scr/Manager.hpp ../Scr/Window.hpp ClacEngine/global.hpp ClacEntity/VeryLong.hpp \
	ClacEngine/cstack.hpp ClacEntity/Entity.hpp ClacEngine/WordStream.hpp 

DirectoryWindow.o:	DirectoryWindow.cpp ClacEntity/LabeledEntity.hpp ClacEntity/Entity.hpp DirectoryWindow.hpp \
	../Scr/ImageBuffer.hpp ../Scr/scr.hpp ../Scr/Manager.hpp ../Scr/Window.hpp ClacEntity/DirectoryEntity.hpp \
	

record_f.o:	record_f.cpp record_f.h 

StackWindow.o:	StackWindow.cpp ../Scr/scr.hpp StackWindow.hpp ../Scr/ImageBuffer.hpp ../Scr/Manager.hpp \
	../Scr/Window.hpp ClacEngine/cstack.hpp ClacEntity/Entity.hpp ClacEntity/VeryLong.hpp \
	


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
