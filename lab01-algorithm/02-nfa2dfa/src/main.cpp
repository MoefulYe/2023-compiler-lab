#include "./nfa_to_dfa.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  auto nfa = NFA::from_stdin();
  auto dfa = DFA::from_nfa(*nfa)->to_string();
  std::cout << dfa;
  return 0;
}
