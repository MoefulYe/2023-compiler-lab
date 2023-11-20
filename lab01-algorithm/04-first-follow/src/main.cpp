#include "../../common/CfgParser.hpp"
#include "./first_follow.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

std::string read_file_to_string(const char *filename) {
  auto file = std::ifstream(filename);
  auto buf = std::ostringstream();
  char ch;
  while (buf && file.get(ch)) {
    buf.put(ch);
  }
  return buf.str();
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  auto cfg = CfgParser(read_file_to_string(argv[1])).parse();
  auto firsts = solve_firsts(cfg);
  for (auto [symbol, first] : firsts) {
    std::cout << "first(" << symbol << ") = " << first.to_string() << std::endl;
  }
  auto follows = solve_follows(cfg, firsts);
  for (auto [symbol, follow] : follows) {
    std::cout << "follow(" << symbol << ") = " << follow.to_string()
              << std::endl;
  }
  return 0;
}
