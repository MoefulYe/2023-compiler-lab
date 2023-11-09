#ifndef CFG_HPP
#define CFG_HPP

#include <cassert>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// 解析不允许有任何空格符
// 1. 产生式的左部和右部用 "->" 分隔
// 2. 右部的多个候选项用 "|" 分隔
// 3. 终结符用单个小写字母表示
// 4. 非终结符用单个大写字母表示
// 5. ~ 表示空串

using std::optional;
using std::string;
using std::string_view;
using std::unordered_map;
using std::vector;

static constexpr string_view EPSILON = "~";
static constexpr string_view OR = "|";
static constexpr string_view ARROW = "->";

struct Symbol {
  char symbol;
  virtual optional<string_view> match(string_view input) = 0;
  virtual ~Symbol() = 0;
  char to_char() { return this->symbol; }
  Symbol(char symbol) : symbol(symbol) {}
};

struct Terminal : public Symbol {
  Terminal(char symbol) : Symbol(symbol) {
    assert(symbol >= 'a' && symbol <= 'z');
  }
  optional<string_view> match(string_view input) override {
    if (input.empty()) {
      return {};
    } else if (input[0] == symbol) {
      return input.substr(1);
    } else {
      return {};
    }
  }
};

struct Nonterminal : public Symbol {
  using Production = vector<Symbol *>;
  vector<Production> productions;
  Nonterminal(char symbol) : Symbol(symbol) {
    assert(symbol >= 'A' && symbol <= 'Z');
  }
  optional<string_view> match(string_view input) override { return {}; }
  string to_expr() {
    auto expr = string(1, symbol);
    expr += ARROW;
    for (int i = 0; i < this->productions.size(); i++) {
      auto &production = this->productions.at(i);
      if (i != 0) {
        expr += OR;
      }
      for (int j = 0; j < production.size(); j++) {
        auto symbol = production.at(j);
        expr += symbol->to_char();
      }
    }
    return expr;
  }
};

struct Epsilon : public Symbol {
  optional<string_view> match(string_view input) override { return {input}; }
  Epsilon() : Symbol('~') {}
};

struct ContextFreeGrammar {
  Nonterminal *start;
  vector<Terminal *> terminals;
  vector<Nonterminal *> nonterminals;
};

#endif
