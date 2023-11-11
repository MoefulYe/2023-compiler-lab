#include "./nfa_to_dfa.hpp"

int main(int argc, char *argv[]) {
  auto nfa = NFA::from_stdin();
  auto dfa = DFA::from_nfa(*nfa);
  return 0;
}
