#include "../../common/comm.hpp"
#include "./nfa_from_regexp.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  assert(argc == 2);
  auto file = argv[1];
  auto content = Util::read_file_to_string(file);
  auto regexs = Util::lines(content);
  for (auto regex : regexs) {
    std::cout << regex << std::endl;
    Parser(regex).parse()->to_nfa().alloc_state()->print();
    getchar();
  }
  return 0;
}
