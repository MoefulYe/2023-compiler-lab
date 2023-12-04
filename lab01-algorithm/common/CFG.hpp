#ifndef CFG_HPP
#define CFG_HPP

#include <bitset>
#include <cassert>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using std::bitset;
using std::map;
using std::optional;
using std::string;
using std::string_view;
using std::unordered_map;
using std::vector;

// 解析不允许有任何空格符
// 1. 产生式的左部和右部用 "->" 分隔
// 2. 右部的多个候选项用 "|" 分隔
// 3. 终结符用单个小写字母表示
// 4. 非终结符用单个大写字母表示
// 5. ~ 表示空产生式
struct ContextFreeGrammar {
  static constexpr int MAX_TERMINAL_NUM = 26;
  static constexpr int MAX_NONTERMINAL_NUM = 26;
  using Symbol = char;
  using ProductionRight = string;
  using ProductionRights = vector<string>;
  using Productions = unordered_map<Symbol, ProductionRights>;
  static constexpr Symbol EPSILON = '~';
  static constexpr char DIVISION = '|';
  static constexpr char END = '$';
  static constexpr string_view ARROW = "->";
  static constexpr string_view START_PREFIX = "start: ";
  static constexpr string_view NONTERMINAL_SET_PREFIX = "nonterminals: ";
  static constexpr string_view TERMINAL_SET_PREFIX = "terminals: ";

  static bool is_terminal(Symbol symbol) {
    return !is_nonterminal(symbol) && !is_epsilon(symbol);
  };
  static bool is_nonterminal(Symbol symbol) {
    return symbol >= 'A' && symbol <= 'Z';
  }
  static bool is_epsilon(Symbol symbol) { return symbol == EPSILON; }

  struct NonterminalSet : bitset<MAX_NONTERMINAL_NUM> {
    NonterminalSet() {}
    NonterminalSet(string_view nonterminals) {
      bitset<MAX_NONTERMINAL_NUM>::reset();
      for (auto symbol : nonterminals) {
        assert(is_nonterminal(symbol));
        this->set(symbol);
      }
    }
    bool get(Symbol symbol) { return (*this)[symbol - 'A']; }
    void set(Symbol symbol) { bitset<MAX_NONTERMINAL_NUM>::set(symbol - 'A'); }
    void reset(Symbol symbol) {
      bitset<MAX_NONTERMINAL_NUM>::reset(symbol - 'A');
    }
    string to_string(Symbol start) {
      auto ret = string{start};
      for (Symbol s = 'A'; s <= 'Z'; s++) {
        if (this->get(s) && s != start) {
          ret += s;
        }
      }
      return ret;
    }

    void alloc(Symbol symbol) {
      assert(is_nonterminal(symbol));
      assert(!this->get(symbol));
      this->set(symbol);
    }

    Symbol alloc() {
      for (auto i = 'A'; i <= 'Z'; i++) {
        if (!this->get(i)) {
          this->set(i);
          return i;
        }
      }
      assert(false);
    }
  };

  ContextFreeGrammar(Symbol start, string_view nonterminals)
      : _nonterminals(nonterminals) {
    this->_start = start;
    for (auto non_term : nonterminals) {
      this->_productions.insert({non_term, {}});
    }
  }
  ContextFreeGrammar(Symbol start, NonterminalSet nonterminals,
                     Productions productions)
      : _start(start), _nonterminals(nonterminals), _productions(productions) {}

  ProductionRights &produce(Symbol nonterminal) {
    assert(is_nonterminal(nonterminal) && "expect nonterminal symbol");
    return _productions.at(nonterminal);
  }

  // 第一项一定是起始符号
  string nonterminals() { return this->_nonterminals.to_string(this->_start); }

  Symbol start() { return this->_start; }

  string to_string() {
    auto ret = string(START_PREFIX) + this->_start + '\n';
    auto nonterminals = this->nonterminals();
    ret += string(NONTERMINAL_SET_PREFIX) + nonterminals + '\n';
    for (auto left : nonterminals) {
      ret += left;
      ret += ARROW;
      auto &rights = this->produce(left);
      for (int i = 0; i < rights.size(); i++) {
        if (i != 0) {
          ret += DIVISION;
        }
        ret += rights.at(i);
      }
      ret += '\n';
    }
    return ret;
  }

  Symbol alloc_nonterminal() {
    auto new_non = this->_nonterminals.alloc();
    this->_productions.insert({new_non, {}});
    return new_non;
  }

  void alloc_nonterminal(Symbol nonterm) {
    this->_nonterminals.alloc(nonterm);
    this->_productions.insert({nonterm, {}});
  }

  ContextFreeGrammar clone() {
    auto productions = this->_productions;
    auto start = this->_start;
    auto non = this->_nonterminals;
    return ContextFreeGrammar(start, non, productions);
  }

private:
  NonterminalSet _nonterminals;
  Symbol _start;
  Productions _productions;
};

#endif
