#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::move;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::unordered_map;
using std::unordered_set;
using std::vector;

struct NFA {
public:
  static NFA build(string_view symbols) {
    auto opers = unordered_set<char>{'*', '|', '(', ')', '.'};
    auto symbol_set = unordered_set<char>();
    for (char c : symbols) {
      if (opers.find(c) != opers.end()) {
        throw "illegal symbol set";
      } else {
        symbol_set.insert(c);
      }
    }
    return NFA(symbol_set, opers);
  }

private:
  unordered_set<char> symbols;
  unordered_set<char> opers;
  unordered_map<char, int> precedence;
  static constexpr char es = '\0';

  NFA(unordered_set<char> symbols, unordered_set<char> opers)
      : symbols(symbols), opers(opers) {
    this->precedence.insert(std::make_pair('*', 3));
    this->precedence.insert(std::make_pair('.', 2));
    this->precedence.insert(std::make_pair('|', 1));
  }

  bool in_symbols(char c) {
    return this->symbols.find(c) != this->symbols.end();
  }
};
