#ifndef CFG_HPP
#define CFG_HPP

#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// 解析不允许有任何空格符
// 1. 产生式的左部和右部用 "->" 分隔
// 2. 右部的多个候选项用 "|" 分隔
// 3. 终结符用单个小写字母表示
// 4. 非终结符用单个大写字母表示, 后面可以跟随任意数量的'，
// A和A'是不同的非终结符
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
  virtual optional<string_view> match(string_view input) = 0;
  virtual string to_expr() = 0;
};

struct Terminal : public Symbol {
  char symbol;
  Terminal(char symbol) : symbol(symbol) {}
  optional<string_view> match(string_view input) override {
    if (input.empty()) {
      return {};
    } else if (input[0] == symbol) {
      return input.substr(1);
    } else {
      return {};
    }
  }
  string to_expr() override { return string(1, symbol); }
};

struct Nonterminal : public Symbol {
  using Production = vector<Symbol *>;
  string symbol;
  vector<Production> productions;
  Nonterminal(string symbol) : symbol(symbol) {}
  optional<string_view> match(string_view input) override { return {}; }
  string to_expr() override {
    string expr;
    expr += symbol;
    expr += "->";
    for (auto &production : productions) {
      for (auto &symbol : production) {
        expr += symbol->to_expr();
      }
      expr += "|";
    }
    return expr;
  }
};

struct Epsilon : public Symbol {
  optional<string_view> match(string_view input) override { return {input}; }
  string to_expr() override { return string(EPSILON); }
};

struct ContextFreeGrammar {
  Nonterminal *start;
  vector<Terminal *> terminals;
  vector<Nonterminal *> nonterminals;
};

#endif
