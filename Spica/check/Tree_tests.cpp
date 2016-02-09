/****************************************************************************
FILE          : tree_test.h
LAST REVISION : 2003-12-04
SUBJECT       : Program to demonstrate the tree.
PROGRAMMER    : (C) Copyright 2003 by Peter C. Chapin

Please send comments or bug reports to

     Peter C. Chapin
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include "Tree.hpp"

#define MAXCOUNT 10000

int main()
{
  spica::tree<int>           my_tree;
  spica::tree<int>::iterator p;
  std::vector<int>           numbers;
  std::vector<int>::iterator current, last;

  // Create a vector full of random numbers. Add the numbers to the tree.
  std::srand(std::time(NULL));
  for (int i = 0; i < MAXCOUNT; ++i) {
    int number = std::rand();
    numbers.push_back(number);
    std::pair<spica::tree<int>::iterator, bool> result = my_tree.insert(number);
    if (result.second == false) {
      // The tree claims the item was alreading inserted. Is that true?
      bool bad = true;
      for (int j = 0; j < i; ++j) {
        if (numbers[j] == number) bad = false;
      }
      if (bad)
        std::cout << "BAD THING: Tree falsely misreports double insertions!\n";
    }
  }

  // Verify that everything put into the tree can be found.
  for (int i = 0; i < MAXCOUNT; ++i) {
    int number = numbers[i];
    p = my_tree.find(number);
    if (p == my_tree.end())
      std::cout << "BAD THING: Lost item(s)!\n";
  }

  // Verify that forward iteration returns items in sorted order.
  std::sort(numbers.begin(), numbers.end());
  current = numbers.begin();
  last = std::unique(numbers.begin(), numbers.end());
  p = my_tree.begin();

  while (p != my_tree.end()) {
    if (current == last)
      std::cout << "BAD THING: Too many items in tree!\n";
    if (*current != *p)
      std::cout << "BAD THING: Items in wrong order!\n";
    ++current;
    ++p;
  }
  if (current != last)
    std::cout << "BAD THING: Not enough items in tree!\n";

  // Verify that reverse iteration returns items in reverse order.
  // EXTRA CREDIT (5 pts).

  std::random_shuffle(numbers.begin(), last);
  for (current = numbers.begin(); current != last; ++current) {
    p = my_tree.find(*current);
    my_tree.erase(p);
    p = my_tree.find(*current);
    if (p != my_tree.end()) {
      std::cout << "BAD THING: Erasing an item doesn't cause it to vanish!\n";
    }
  }
  if (my_tree.size() != 0) {
    std::cout << "BAD THING: Tree has non-zero size after erasing all.\n";
  }

  std::cout << "Testing complete.\n";
}
