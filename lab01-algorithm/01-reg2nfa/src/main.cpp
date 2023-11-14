#include "./nfa_from_regexp.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  char buf[256];
  while (std::cin.getline(buf, 256)) {
    std::cout << buf << std::endl;
    Parser(buf).parse()->to_nfa().alloc_state()->print();
  }
  return 0;
}
