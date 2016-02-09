#
# Makefile for the Pixie project.
#

CXX=g++
CXXFLAGS=-std=c++11 -c -O -I../Spica -I../Scr
LINK=g++
LINKFLAGS=-lncurses -lpthread
SOURCES=main.cpp               \
	PixieCommandWindow.cpp \
	TaskWindow.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=pixie
LIBSPICA=../Spica/libSpicaCpp.a
LIBSCR=../Scr/libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBSPICA) $(LIBSCR) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:26:23 2013


main.o:	main.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp ../Scr/ImageBuffer.hpp \
	../Scr/Manager.hpp PixieCommandWindow.hpp ../Scr/CommandWindow.hpp TaskWindow.hpp ../Spica/Date.hpp \
	

PixieCommandWindow.o:	PixieCommandWindow.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp \
	../Scr/ImageBuffer.hpp PixieCommandWindow.hpp ../Scr/CommandWindow.hpp ../Scr/Manager.hpp \
	TaskWindow.hpp ../Spica/Date.hpp 

TaskWindow.o:	TaskWindow.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp \
	../Scr/ImageBuffer.hpp TaskWindow.hpp ../Spica/Date.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
