#
# Makefile for the scr library.
#

CXX=g++
CXXFLAGS=-c -O -std=c++11 -I../Spica
LINK=ar
LINKFLAGS=rc
SOURCES=ansiscr.cpp       \
	CommandWindow.cpp \
	debug.cpp         \
	DisplayWindow.cpp \
	ImageBuffer.cpp   \
	InputWindow.cpp   \
	key.cpp           \
	ListWindow.cpp    \
	Manager.cpp       \
	MessageWindow.cpp \
	scr.cpp           \
	scrtools.cpp      \
	SelectWindow.cpp  \
	Shadow.cpp        \
	StatusLine.cpp    \
	TextWindow.cpp    \
	Window.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(LIBRARY):	$(OBJECTS)
	$(LINK) $(LINKFLAGS) -o $@ $(OBJECTS)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sun Jul 14 14:46:07 2013

ansiscr.o:	ansiscr.cpp ansiscr.hpp 

CommandWindow.o:	CommandWindow.cpp CommandWindow.hpp ImageBuffer.hpp scr.hpp Manager.hpp Window.hpp \
	scrtools.hpp 

debug.o:	debug.cpp debug.hpp scr.hpp TextWindow.hpp Window.hpp ImageBuffer.hpp scrtools.hpp \
	

DisplayWindow.o:	DisplayWindow.cpp DisplayWindow.hpp Window.hpp ImageBuffer.hpp scr.hpp 

ImageBuffer.o:	ImageBuffer.cpp ImageBuffer.hpp scr.hpp 

InputWindow.o:	InputWindow.cpp InputWindow.hpp Shadow.hpp Window.hpp ImageBuffer.hpp scr.hpp scrtools.hpp \
	

key.o:	key.cpp ../Spica/environ.hpp scr.hpp 

ListWindow.o:	ListWindow.cpp ListWindow.hpp ImageBuffer.hpp scr.hpp Manager.hpp Window.hpp scrtools.hpp \
	

Manager.o:	Manager.cpp ImageBuffer.hpp scr.hpp Manager.hpp Window.hpp scrtools.hpp 

MessageWindow.o:	MessageWindow.cpp MessageWindow.hpp Shadow.hpp Window.hpp ImageBuffer.hpp scr.hpp \
	scrtools.hpp 

scr.o:	scr.cpp ../Spica/environ.hpp scr.hpp ansiscr.hpp 

scrtools.o:	scrtools.cpp scr.hpp scrtools.hpp 

SelectWindow.o:	SelectWindow.cpp scr.hpp scrtools.hpp SelectWindow.hpp DisplayWindow.hpp Window.hpp \
	ImageBuffer.hpp 

Shadow.o:	Shadow.cpp scr.hpp Shadow.hpp 

StatusLine.o:	StatusLine.cpp scr.hpp StatusLine.hpp Window.hpp ImageBuffer.hpp 

TextWindow.o:	TextWindow.cpp scr.hpp scrtools.hpp TextWindow.hpp Window.hpp ImageBuffer.hpp 

Window.o:	Window.cpp Manager.hpp Window.hpp ImageBuffer.hpp scr.hpp scrtools.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) *.s *.ll *~
