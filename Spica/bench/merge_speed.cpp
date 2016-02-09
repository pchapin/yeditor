/**************************************************************************
FILE          : merge_speed.cpp
PROGRAMMER    : Peter C. Chapin
LAST REVISION : 2003-08-31

(C) Copyright 2003 by Peter Chapin

This file contains a program that measures the performance of merge
sort for various sequence lengths. It is intended to demonstrate the O(nlgn)
behavior of the algorithm.

Please send comments or bug reports to

     Peter C. Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
**************************************************************************/

#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include "merge_sort.h"
#include "timer.h"

// A couple of useful constants.
const int K = 1024;
const int M = K*K;

//
// Main program just exercises each test.
//
int main()
{
  int sequence_size;

  // We don't need to seed the random number generator randomly.
  std::srand(0);

  std::cout << std::setiosflags(std::ios::fixed);

  // Do the tests.
  for (sequence_size = 256; sequence_size <= 1*M; sequence_size *= 2) {
    pcc::Timer stopwatch;

    // Allocate our test array and fill it with random data.
    int *p = new int[sequence_size];
    for (int i = 0; i < sequence_size; ++i) p[i] = std::rand();

    // Sort the sequence, timing it as we do so.
    stopwatch.start();
    VTC::merge_sort(p, p + sequence_size, std::less<int>());
    stopwatch.stop();

    // What is the result?
    std::cout <<   "Size = " << std::setw(9) << sequence_size 
              << "; Time = " << std::setw(9) << std::setprecision(3)
              << stopwatch.time()/1000.0 << "s\n";

    delete [] p;
  }

  return 0;
}

