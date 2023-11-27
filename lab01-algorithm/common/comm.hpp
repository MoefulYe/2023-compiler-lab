#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Util {
  static std::string read_file_to_string(std::string_view filename) {
    std::ifstream file(filename.data());
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
  }

  static std::vector<std::string_view> lines(std::string_view input) {
    auto ret = std::vector<std::string_view>();
    while (!input.empty()) {
      auto pos = input.find('\n');
      if (pos == std::string_view::npos) {
        pos = input.size();
      }
      if (auto to_push = input.substr(0, pos); to_push.substr(0, 1) != "#") {
        ret.push_back(to_push);
      }
      input = input.substr(pos < input.size() ? pos + 1 : pos);
    }
    return ret;
  }

  static std::vector<std::string_view> split(std::string_view input,
                                             char delim) {
    auto ret = std::vector<std::string_view>();
    while (!input.empty()) {
      auto pos = input.find(delim);
      if (pos == std::string_view::npos) {
        pos = input.size();
      }
      ret.push_back(input.substr(0, pos));
      input = input.substr(pos < input.size() ? pos + 1 : pos);
    }
    return ret;
  }

  static int string_view2int(std::string_view input) {
    int sgn;
    if (input.substr(0, 1) == "-") {
      sgn = -1;
      input = input.substr(1);
    } else if (input.substr(0, 1) == "+") {
      sgn = 1;
      input = input.substr(1);
    } else {
      sgn = 1;
    }
    int ret = 0;
    for (auto c : input) {
      assert(c >= '0' && c <= '9');
      ret = ret * 10 + c - '0';
    }
    return ret * sgn;
  }
};

#endif // ! FILE_UTIL_H
