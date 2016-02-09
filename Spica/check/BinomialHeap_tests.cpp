/*! \file    BinomialHeap_tests.cpp
    \brief   Exercise spica::BinomialHeap.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include <spica.hpp>
#include "BinomialHeap.hpp"
#include "u_tests.hpp"
#include "UnitTestManager.hpp"

#define N 10000  // The number of items in the heap during primary testing.

std::vector<int> make_numbers(int count)
{
  std::vector<int> result;
  std::srand(std::time(0));
  for (int i = 0; i < count; ++i) {
    result.push_back(std::rand());
  }
  return result;
}

void check_heap(
  spica::BinomialHeap<int> &my_heap, std::vector<int> &numbers)
{
  std::sort(numbers.begin(), numbers.end());

  spica::BinomialHeap<int>::iterator p_heap;
  for (p_heap = my_heap.begin(); p_heap != my_heap.end(); ++p_heap) {
    UNIT_CHECK(std::binary_search(numbers.begin(), numbers.end(), *p_heap));
  }

  std::vector<int>::iterator p_vec;
  for (p_vec = numbers.begin(); p_vec != numbers.end(); ++p_vec) {
    int  value = my_heap.front();

    UNIT_CHECK(value == *p_vec);
    my_heap.pop();
  }
  UNIT_CHECK(my_heap.size() == 0);
}


bool BinomialHeap_tests()
{
  std::vector<int> numbers1 = make_numbers(N);
  spica::BinomialHeap<int> heap1(numbers1.begin(), numbers1.end());
  UNIT_CHECK(heap1.size() == N);
  check_heap(heap1, numbers1);

  numbers1 = make_numbers(N);
  heap1.insert(numbers1.begin(), numbers1.end());
  std::vector<int> numbers2 = make_numbers(N);
  spica::BinomialHeap<int> heap2(numbers2.begin(), numbers2.end());

  heap1.merge(heap2);
  numbers1.insert(numbers1.end(), numbers2.begin(), numbers2.end());
  UNIT_CHECK(heap1.size() == 2*N);
  check_heap(heap1, numbers1);

  return true;
}
