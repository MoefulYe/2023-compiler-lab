#include "../common/CFG.hpp"
#include "../common/CfgParser.hpp"
#include "lf/lf.h"
#include "lrk/lrk.h"
#include <cassert>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  assert(argc > 1);
  for (int i = 1; i < argc; i++) {
    auto file = argv[i];
    auto cfg = CfgParser(Util::read_file_to_string(file)).parse();
    left_recursion_kill(cfg);
    std::cout << cfg.to_string() << std::endl;
    extract_left_factor(cfg);
    std::cout << cfg.to_string() << std::endl;
    getchar();
  }
  return 0;
}
