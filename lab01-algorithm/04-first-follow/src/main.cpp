#include "../../03-cfg-trans/lrk/lrk.h"
#include "../../common/CfgParser.hpp"
#include "../../common/comm.hpp"
#include "./first_follow.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

int main(int argc, char *argv[]) {
  assert(argc > 1);
  for (int i = 1; i < argc; i++) {
    auto file = argv[i];
    auto cfg = CfgParser(Util::read_file_to_string(file)).parse();
    auto nonterminals = cfg.nonterminals();
    left_recursion_kill(cfg);
    std::cout << cfg.to_string() << std::endl;
    std::cout << "|symbol\t\t|first\t\t|follow\t\t|" << std::endl;
    auto firsts = solve_firsts(cfg);
    auto follows = solve_follows(cfg, firsts);
    for (auto symbol : nonterminals) {
      std::cout << "|" << symbol << "\t\t";
      std::cout << "|" << firsts.at(symbol).to_string() << "\t\t";
      std::cout << "|" << follows.at(symbol).to_string() << "\t\t|"
                << std::endl;
    }
    getchar();
  }
  return 0;
}
