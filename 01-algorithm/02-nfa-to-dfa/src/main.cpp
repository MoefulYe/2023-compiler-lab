#include "./nfa_to_dfa.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  char buf[256];
  std::cin.getline(buf, 256);
  auto set = Bitset::from_string(buf);
  auto start = atoi(buf);
  std::cin.getline(buf, 256);
  auto end = atoi(buf);
  std::cin.getline(buf, 256);
  auto total = atoi(buf);
  std::cin.getline(buf, 256);
  auto symbols = std::string(buf);
  auto nfa = NFA(start, end, total);
  for (int i = 0; i < total; i++) {
    std::cin.getline(buf, 256);
    auto tokens = split_line(buf);
    auto to = NFA::State::Trans();
    for (int j = 0; j < tokens.size(); j++) {
      to[symbols[j]] = Bitset::from_string(tokens[j]);
    }
    nfa.states.push_back(NFA::State{to});
  }

  auto dfa = DFA::from_nfa(nfa, symbols);
  return 0;
}
