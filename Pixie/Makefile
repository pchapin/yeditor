#
# Makefile for the Pixie project.
#

CXX=clang++
CXXFLAGS=-std=c++11 -c -O -emit-llvm -I../Spica/Cpp -I../Scr
LLVMLINK=llvm-link
LLVMLINKFLAGS=
# It shouldn't be necessary to use Boost here since my application does not require it.
LINKFLAGS=-L/usr/lib/i386-linux-gnu -L$(BOOST_HOME)/stage/lib -lboost_regex -lncurses -lpthread
SOURCES=main.cpp               \
	PixieCommandWindow.cpp \
	TaskWindow.cpp
OBJECTS=$(SOURCES:.cpp=.bc)
EXECUTABLE=Pixie
LIBSPICA=../Spica/Cpp/libSpicaCpp.bc
LIBSCR=../Scr/libScr.bc

%.bc:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	Pixie.o
	$(CXX) $< $(LINKFLAGS) -o $@

Pixie.o:	Pixie.bc2
	llc -filetype=obj Pixie.bc2 -o Pixie.o

Pixie.bc2:	Pixie.bc1
	opt -o Pixie.bc2 -globaldce Pixie.bc1

Pixie.bc1:	$(OBJECTS) $(LIBSPICA) $(LIBSCR)
	$(LLVMLINK) $(LLVMLINKFLAGS) -o $@ $(OBJECTS) $(LIBSPICA) $(LIBSCR)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:26:23 2013


main.o:	main.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp ../Scr/ImageBuffer.hpp \
	../Scr/Manager.hpp PixieCommandWindow.hpp ../Scr/CommandWindow.hpp TaskWindow.hpp ../Spica/Cpp/Date.hpp \
	

PixieCommandWindow.o:	PixieCommandWindow.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp \
	../Scr/ImageBuffer.hpp PixieCommandWindow.hpp ../Scr/CommandWindow.hpp ../Scr/Manager.hpp \
	TaskWindow.hpp ../Spica/Cpp/Date.hpp 

TaskWindow.o:	TaskWindow.cpp ../Scr/debug.hpp ../Scr/scr.hpp ../Scr/TextWindow.hpp ../Scr/Window.hpp \
	../Scr/ImageBuffer.hpp TaskWindow.hpp ../Spica/Cpp/Date.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
