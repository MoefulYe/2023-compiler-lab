#include "./nfa_to_dfa.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  assert(argc > 1);
  for (int i = 1; i < argc; i++) {
    auto line = argv[i];
    auto nfa = NFA::from_str(Util::read_file_to_string(line));
    std::cout << DFA::from_nfa(*nfa)->to_string() << std::endl;
    getchar();
  }
}
