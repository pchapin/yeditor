/****************************************************************************
FILE          : verylong_speed.cpp
LAST REVISION : 2002-12-21
SUBJECT       : Program to test performance of pcc::very_long integers.
PROGRAMMER    : (C) Copyright 2002 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include <cstdlib>
#include <iostream>
#include "timer.h"
#include "verylong.h"

const int N_TRIALS = 256;

int main()
{
  std::cout << "\nMultiplication\n";
  std::cout <<   "==============\n";
  for (int n_bits = 256; n_bits <= 16384; n_bits *= 2) {

    // Allocate the necessary arrays.
    pcc::very_long *m1 = new pcc::very_long[N_TRIALS];
    pcc::very_long *m2 = new pcc::very_long[N_TRIALS];

    // Fill the the input arrays with random numbers, each n_bits in size.
    for (int i = 0; i < N_TRIALS; ++i) {
      for (pcc::very_long::size_type j = 0; j < n_bits - 1; ++j) {
        m1[i].put_bit(j, std::rand() % 2);
        m2[i].put_bit(j, std::rand() % 2);
      }
      // Make sure most significant bits are one.
      m1[i].put_bit(n_bits - 1, 1);
      m2[i].put_bit(n_bits - 1, 1);
    }

    pcc::Timer stopwatch;
    stopwatch.start();
    for (int i = 0; i < N_TRIALS; ++i) {
      pcc::very_long result = m1[i] * m2[i];
    }
    stopwatch.stop();
    std::cout
      << n_bits << " bits: " << stopwatch.time()/N_TRIALS << " ms.\n";

    // Clean up.
    delete [] m1;
    delete [] m2;
  }

  std::cout << "\nDivision\n";
  std::cout <<   "========\n";
  for (int n_bits = 256; n_bits <= 16384; n_bits *= 2) {

    // Allocate the necessary arrays.
    pcc::very_long *m1 = new pcc::very_long[N_TRIALS];
    pcc::very_long *m2 = new pcc::very_long[N_TRIALS];

    // Fill the the input arrays with random numbers.
    for (int i = 0; i < N_TRIALS; ++i) {

      // Make the numerator 2*n_bits in size so division requires n_bits steps.
      for (pcc::very_long::size_type j = 0; j < 2*n_bits - 1; ++j) {
        m1[i].put_bit(j, std::rand() % 2);
      }
      m1[i].put_bit(2*n_bits - 1, 1);
      for (pcc::very_long::size_type j = 0; j < n_bits - 1; ++j) {
        m2[i].put_bit(j, std::rand() % 2);
      }
      m2[i].put_bit(n_bits - 1, 1);
    }

    pcc::Timer stopwatch;
    stopwatch.start();
    for (int i = 0; i < N_TRIALS; ++i) {
      pcc::very_long::vldiv_t result;
      pcc::very_long::vldiv(m1[i], m2[i], &result);
    }
    stopwatch.stop();
    std::cout
      << n_bits << " bits: " << stopwatch.time()/N_TRIALS << " ms.\n";

    // Clean up.
    delete [] m1;
    delete [] m2;
  }

  return 0;
}
