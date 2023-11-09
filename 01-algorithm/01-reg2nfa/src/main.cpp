#include "./nfa_from_regexp.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  string input;
  std::cin >> input;
  Parser(input).parse()->to_nfa().alloc_state()->print();
  return 0;
}
