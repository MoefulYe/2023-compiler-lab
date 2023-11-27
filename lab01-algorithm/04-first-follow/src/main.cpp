#include "../../common/CfgParser.hpp"
#include "../../common/comm.hpp"
#include "./first_follow.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

int main(int argc, char *argv[]) {
  assert(argc > 1);
  for (int i = 1; i < argc; i++) {
    auto file = argv[i];
    auto cfg = CfgParser(Util::read_file_to_string(argv[1])).parse();
    auto nonterminals = cfg.nonterminals();
    std::cout << "symbol\t\t|first|\t\t|follow\t\t|" << std::endl;
    auto firsts = solve_firsts(cfg);
    auto follows = solve_follows(cfg, firsts);
    for (auto symbol : nonterminals) {
      std::cout << symbol << "\t\t";
      std::cout << firsts.at(symbol).to_string() << "\t\t";
      std::cout << follows.at(symbol).to_string() << "\t\t";
      std::cout << std::endl;
    }
  }
  return 0;
}
