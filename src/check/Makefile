#
# Makefile for the Check project.
#

CXX=clang++
CXXFLAGS=-std=c++11 -c -O -emit-llvm -I../../../Spica/Cpp -I..
LLVMLINK=llvm-link
LLVMLINKFLAGS=
# It shouldn't be necessary to use Boost here since my application does not require it.
LINKFLAGS=-L/usr/lib/i386-linux-gnu -L$(BOOST_HOME)/stage/lib -lboost_regex -lncurses -lpthread
SOURCES=check.cpp
OBJECTS=$(SOURCES:.cpp=.bc)
OBJECTSTESTED=../EditBuffer.bc
EXECUTABLE=check
LIBSPICA=../../../Spica/Cpp/libSpicaCpp.bc
LIBSCR=../../../Scr/libScr.bc

%.bc:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(EXECUTABLE):	check.o
	$(CXX) $< $(LINKFLAGS) -o $@

check.o:	check.bc2
	llc -filetype=obj check.bc2 -o check.o

check.bc2:	check.bc1
	opt -o check.bc2 -globaldce check.bc1

check.bc1:	$(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR)
	$(LLVMLINK) $(LLVMLINKFLAGS) -o $@ $(OBJECTS) $(OBJECTSTESTED) $(LIBSPICA) $(LIBSCR)

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Sat Jul  6 17:55:04 2013


check.bc:	check.cpp ../EditBuffer.hpp ../../../Spica/Cpp/UnitTestManager.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.bc1 *.bc2 *.o $(EXECUTABLE) *.s *.ll *~
