#include "../../common/CfgParser.hpp"
#include "./lf.h"
#include <algorithm>
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
  for (int i = 1; i < argc; i++) {
    auto cfg = CfgParser(read_file_to_string(argv[i])).parse();
    extract_left_factor(cfg);
    std::cout << cfg.to_string() << std::endl;
  }
  return 0;
}
