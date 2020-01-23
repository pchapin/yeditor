#
# Makefile for the Y project.
#

CXX=g++
CXXFLAGS=-Wall -std=c++11 -c -O -I../scr
LINK=g++
LINKFLAGS=-lncurses
## It shouldn't be necessary to use Boost here since my application does not require it.
#LINKFLAGS=-L/usr/lib/i386-linux-gnu -L$(BOOST_HOME)/stage/lib -lboost_regex -lncurses -lpthread
SOURCES=BlockEditFile.cpp     \
	CharacterEditFile.cpp \
	clipboard.cpp         \
	command_a.cpp         \
	command_b.cpp         \
	command_c.cpp         \
	command_d.cpp         \
	command_e.cpp         \
	command_f.cpp         \
	command_g.cpp         \
	command_h.cpp         \
	command_i.cpp         \
	command_k.cpp         \
	command_l.cpp         \
	command_n.cpp         \
	command_p.cpp         \
	command_q.cpp         \
	command_r.cpp         \
	command_s.cpp         \
	command_table.cpp     \
	command_t.cpp         \
	command_x.cpp         \
	command_y.cpp         \
	CursorEditFile.cpp    \
	DiskEditFile.cpp      \
	EditBuffer.cpp        \
	EditFile.cpp          \
	EditList.cpp          \
	FileList.cpp          \
	FileNameMatcher.cpp   \
	FilePosition.cpp      \
	global.cpp            \
	help.cpp              \
	keyboard.cpp          \
	LineEditFile.cpp      \
	macro_stack.cpp       \
	parameter_stack.cpp   \
	SearchEditFile.cpp    \
	special.cpp           \
	support.cpp           \
	Timer.cpp             \
	WordSource.cpp        \
	WPEditFile.cpp        \
	y.cpp                 \
	YEditFile.cpp         \
	yfile.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Y
LIBSCR=../scr/libScr.a

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBSCR) $(LINKFLAGS) -o $@


# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Wed Jan 22 20:36:01 2020


BlockEditFile.o:	BlockEditFile.cpp BlockEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp \
	EditBuffer.hpp support.hpp ../scr/environ.hpp 

CharacterEditFile.o:	CharacterEditFile.cpp EditBuffer.hpp CharacterEditFile.hpp EditFile.hpp EditList.hpp \
	mylist.hpp FilePosition.hpp support.hpp ../scr/environ.hpp 

clipboard.o:	clipboard.cpp clipboard.hpp EditList.hpp mylist.hpp 

command_a.o:	command_a.cpp command.hpp FileList.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp \
	mylist.hpp mystack.hpp yfile.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp \
	SearchEditFile.hpp WPEditFile.hpp 

command_b.o:	command_b.cpp FileList.hpp global.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp \
	mylist.hpp mystack.hpp support.hpp ../scr/environ.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp \
	FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp \
	SearchEditFile.hpp WPEditFile.hpp 

command_c.o:	command_c.cpp clipboard.hpp EditList.hpp mylist.hpp FileList.hpp yfile.hpp EditBuffer.hpp \
	YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp \
	DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_d.o:	command_d.cpp clipboard.hpp EditList.hpp mylist.hpp command.hpp FileList.hpp parameter_stack.hpp \
	EditBuffer.hpp mystack.hpp WordSource.hpp yfile.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp \
	FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp ../scr/environ.hpp \
	LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_e.o:	command_e.cpp command.hpp FileList.hpp global.hpp parameter_stack.hpp EditBuffer.hpp \
	EditList.hpp mylist.hpp mystack.hpp help.hpp macro_stack.hpp WordSource.hpp ../scr/scr.hpp \
	support.hpp ../scr/environ.hpp yfile.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp 

command_f.o:	command_f.cpp command.hpp EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp FileList.hpp \
	global.hpp parameter_stack.hpp EditBuffer.hpp mystack.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp \
	YEditFile.hpp BlockEditFile.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp \
	LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp yfile.hpp 

command_g.o:	command_g.cpp FileList.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp mylist.hpp \
	mystack.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp CharacterEditFile.hpp \
	CursorEditFile.hpp DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp 

command_h.o:	command_h.cpp command.hpp help.hpp 

command_i.o:	command_i.cpp command.hpp FileList.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp \
	mylist.hpp mystack.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp CharacterEditFile.hpp \
	CursorEditFile.hpp DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp 

command_k.o:	command_k.cpp command.hpp FileList.hpp support.hpp ../scr/environ.hpp EditBuffer.hpp \
	YEditFile.hpp BlockEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp CharacterEditFile.hpp \
	CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_l.o:	command_l.cpp command.hpp help.hpp 

command_n.o:	command_n.cpp command.hpp FileList.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp \
	EditList.hpp mylist.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp \
	../scr/environ.hpp EditBuffer.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_p.o:	command_p.cpp clipboard.hpp EditList.hpp mylist.hpp command.hpp FileList.hpp YEditFile.hpp \
	BlockEditFile.hpp EditFile.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp \
	DiskEditFile.hpp ../scr/environ.hpp EditBuffer.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp \
	

command_q.o:	command_q.cpp command.hpp FileList.hpp support.hpp ../scr/environ.hpp EditBuffer.hpp \
	

command_r.o:	command_r.cpp command.hpp FileList.hpp global.hpp parameter_stack.hpp EditBuffer.hpp \
	EditList.hpp mylist.hpp mystack.hpp help.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp \
	yfile.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp CharacterEditFile.hpp \
	CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_s.o:	command_s.cpp command.hpp FileList.hpp global.hpp parameter_stack.hpp EditBuffer.hpp \
	EditList.hpp mylist.hpp mystack.hpp ../scr/MessageWindow.hpp ../scr/Shadow.hpp ../scr/Window.hpp \
	../scr/ImageBuffer.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp YEditFile.hpp BlockEditFile.hpp \
	EditFile.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp \
	LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_table.o:	command_table.cpp command.hpp command_table.hpp EditBuffer.hpp parameter_stack.hpp \
	EditList.hpp mylist.hpp mystack.hpp support.hpp ../scr/environ.hpp 

command_t.o:	command_t.cpp command.hpp FileList.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp \
	EditList.hpp mylist.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp \
	../scr/environ.hpp EditBuffer.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

command_x.o:	command_x.cpp command.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp mylist.hpp \
	mystack.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp 

command_y.o:	command_y.cpp command.hpp FileList.hpp yfile.hpp EditBuffer.hpp mylist.hpp YEditFile.hpp \
	BlockEditFile.hpp EditFile.hpp EditList.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp \
	DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

CursorEditFile.o:	CursorEditFile.cpp EditBuffer.hpp CursorEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp \
	FilePosition.hpp 

DiskEditFile.o:	DiskEditFile.cpp ../scr/environ.hpp DiskEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp \
	FilePosition.hpp EditBuffer.hpp FileNameMatcher.hpp ../scr/MessageWindow.hpp ../scr/Shadow.hpp \
	../scr/Window.hpp ../scr/ImageBuffer.hpp ../scr/scr.hpp support.hpp 

EditBuffer.o:	EditBuffer.cpp EditBuffer.hpp 

EditFile.o:	EditFile.cpp EditBuffer.hpp EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp \
	support.hpp ../scr/environ.hpp 

EditList.o:	EditList.cpp EditBuffer.hpp EditList.hpp mylist.hpp 

FileList.o:	FileList.cpp EditBuffer.hpp FileList.hpp FileNameMatcher.hpp ../scr/environ.hpp mylist.hpp \
	special.hpp ../scr/scr.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp EditList.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp support.hpp yfile.hpp 

FileNameMatcher.o:	FileNameMatcher.cpp ../scr/environ.hpp FileNameMatcher.hpp 

FilePosition.o:	FilePosition.cpp FilePosition.hpp ../scr/scr.hpp 

global.o:	global.cpp ../scr/environ.hpp global.hpp parameter_stack.hpp EditBuffer.hpp EditList.hpp \
	mylist.hpp mystack.hpp ../scr/scr.hpp support.hpp 

help.o:	help.cpp help.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp EditBuffer.hpp ../scr/TextWindow.hpp \
	../scr/Window.hpp ../scr/ImageBuffer.hpp 

keyboard.o:	keyboard.cpp command.hpp FileList.hpp keyboard.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp \
	EditBuffer.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp 

LineEditFile.o:	LineEditFile.cpp EditBuffer.hpp LineEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp \
	FilePosition.hpp support.hpp ../scr/environ.hpp 

macro_stack.o:	macro_stack.cpp EditBuffer.hpp macro_stack.hpp mystack.hpp mylist.hpp WordSource.hpp \
	

parameter_stack.o:	parameter_stack.cpp EditBuffer.hpp global.hpp parameter_stack.hpp EditList.hpp mylist.hpp \
	mystack.hpp ../scr/scr.hpp ../scr/Shadow.hpp support.hpp ../scr/environ.hpp ../scr/Window.hpp \
	../scr/ImageBuffer.hpp 

SearchEditFile.o:	SearchEditFile.cpp EditBuffer.hpp SearchEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp \
	FilePosition.hpp 

special.o:	special.cpp EditBuffer.hpp ../scr/scr.hpp special.hpp YEditFile.hpp BlockEditFile.hpp \
	EditFile.hpp EditList.hpp mylist.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp \
	DiskEditFile.hpp ../scr/environ.hpp LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp support.hpp \
	

support.o:	support.cpp ../scr/environ.hpp FileList.hpp FileNameMatcher.hpp global.hpp parameter_stack.hpp \
	EditBuffer.hpp EditList.hpp mylist.hpp mystack.hpp Timer.hpp ../scr/MessageWindow.hpp ../scr/Shadow.hpp \
	../scr/Window.hpp ../scr/ImageBuffer.hpp ../scr/scr.hpp support.hpp YEditFile.hpp BlockEditFile.hpp \
	EditFile.hpp FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp \
	LineEditFile.hpp SearchEditFile.hpp WPEditFile.hpp 

Timer.o:	Timer.cpp Timer.hpp ../scr/environ.hpp 

WordSource.o:	WordSource.cpp EditBuffer.hpp keyboard.hpp macro_stack.hpp mystack.hpp mylist.hpp \
	WordSource.hpp parameter_stack.hpp EditList.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp \
	

WPEditFile.o:	WPEditFile.cpp EditBuffer.hpp support.hpp ../scr/environ.hpp WPEditFile.hpp EditFile.hpp \
	EditList.hpp mylist.hpp FilePosition.hpp 

y.o:	y.cpp command.hpp command_table.hpp EditBuffer.hpp FileList.hpp FileNameMatcher.hpp \
	../scr/environ.hpp global.hpp parameter_stack.hpp EditList.hpp mylist.hpp mystack.hpp ../scr/MessageWindow.hpp \
	../scr/Shadow.hpp ../scr/Window.hpp ../scr/ImageBuffer.hpp ../scr/scr.hpp macro_stack.hpp \
	WordSource.hpp support.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp yfile.hpp 

YEditFile.o:	YEditFile.cpp EditBuffer.hpp FileList.hpp ../scr/scr.hpp ../scr/scrtools.hpp support.hpp \
	../scr/environ.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp EditList.hpp mylist.hpp \
	FilePosition.hpp CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp \
	SearchEditFile.hpp WPEditFile.hpp yfile.hpp 

yfile.o:	yfile.cpp FileList.hpp ../scr/scr.hpp support.hpp ../scr/environ.hpp EditBuffer.hpp \
	yfile.hpp mylist.hpp YEditFile.hpp BlockEditFile.hpp EditFile.hpp EditList.hpp FilePosition.hpp \
	CharacterEditFile.hpp CursorEditFile.hpp DiskEditFile.hpp LineEditFile.hpp SearchEditFile.hpp \
	WPEditFile.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
