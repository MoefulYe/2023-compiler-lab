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
  using TerminalSet = bitset<MAX_TERMINAL_NUM>;
  using NonterminalSet = bitset<MAX_NONTERMINAL_NUM>;
  static constexpr Symbol EPSILON = '~';
  static constexpr char DIVISION = '|';
  static constexpr string_view ARROW = "->";
  static constexpr string_view START_PREFIX = "start: ";
  static constexpr string_view NONTERMINAL_SET_PREFIX = "nonterminals: ";
  static constexpr string_view TERMINAL_SET_PREFIX = "terminals: ";

  static bool is_terminal(Symbol symbol) {
    return symbol >= 'a' && symbol <= 'z';
  };
  static bool is_nonterminal(Symbol symbol) {
    return symbol >= 'A' && symbol <= 'Z';
  }
  static bool is_epsilon(Symbol symbol) { return symbol == EPSILON; }

  ContextFreeGrammar(Symbol start, string_view nonterminals,
                     string_view terminals) {
    this->set_terminals(terminals);
    this->set_nonterminals(nonterminals);
    this->_start = start;
    for (auto non_term : nonterminals) {
      this->_productions.insert({non_term, {}});
    }
  }

  ProductionRights &produce(Symbol nonterminal) {
    assert(is_nonterminal(nonterminal) && "expect nonterminal symbol");
    return _productions.at(nonterminal);
  }

  string get_terminals() {
    auto ret = string();
    for (int i = 0; i < this->_terminals.size(); i++) {
      if (_terminals[i]) {
        ret.push_back(i + 'a');
      }
    }
    return ret;
  }

  // 第一项一定是起始符号
  string get_nonterminals() {
    auto ret = string{this->_start};
    for (int i = 0; i < this->_nonterminals.size(); i++) {
      if (_nonterminals[i]) {
        auto symbol = i + 'A';
        if (symbol != this->_start) {
          ret.push_back(symbol);
        }
      }
    }
    return ret;
  }

  void set_terminals(string_view terminals) {
    this->_terminals.reset();
    for (auto terminal : terminals) {
      this->_terminals.set(terminal - 'a');
    }
  }

  void set_nonterminals(string_view nonterminals) {
    this->_nonterminals.reset();
    for (auto nonterminal : nonterminals) {
      this->_nonterminals.set(nonterminal - 'A');
    }
  }

  Symbol start() { return this->_start; }

  string to_string() {
    auto ret = string(START_PREFIX) + this->_start + '\n';
    auto terminals = this->get_terminals();
    auto nonterminals = this->get_nonterminals();
    ret += nonterminals + '\n';
    ret += terminals + '\n';
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
    for (int i = 0; i < this->_nonterminals.size(); i++) {
      if (auto symbol = i + 'A'; !_nonterminals[i]) {
        _nonterminals.set(i);
        _productions.insert({symbol, {}});
        return symbol;
      }
    }
    assert(false);
  }

  void alloc_nonterminal(Symbol nonterm) {
    assert(is_nonterminal(nonterm));
    assert(!this->_nonterminals[nonterm - 'A']);
    this->_nonterminals.set(nonterm - 'A');
  }

private:
  TerminalSet _terminals;
  NonterminalSet _nonterminals;
  Symbol _start;
  Productions _productions;
};

#endif
