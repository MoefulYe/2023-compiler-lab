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
  static constexpr Symbol EPSILON = '~';
  static constexpr char DIVISION = '|';
  static constexpr char END = '$';
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

  struct TerminalSet : bitset<MAX_TERMINAL_NUM> {
    TerminalSet() {}
    TerminalSet(string_view terminals) {
      bitset<MAX_TERMINAL_NUM>::reset();
      for (auto symbol : terminals) {
        assert(is_terminal(symbol));
        this->set(symbol);
      }
    }
    bool get(Symbol symbol) { return (*this)[symbol - 'a']; }
    void set(Symbol symbol) { bitset<MAX_TERMINAL_NUM>::set(symbol - 'a'); }
    void reset(Symbol symbol) { bitset<MAX_TERMINAL_NUM>::reset(symbol - 'a'); }
    string to_string() {
      auto ret = string();
      for (Symbol s = 'a'; s <= 'z'; s++) {
        if (this->get(s)) {
          ret += s;
        }
      }
      return ret;
    }
  };

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

  ContextFreeGrammar(Symbol start, string_view nonterminals,
                     string_view terminals)
      : _terminals(terminals), _nonterminals(nonterminals) {
    this->_start = start;
    for (auto non_term : nonterminals) {
      this->_productions.insert({non_term, {}});
    }
  }

  ProductionRights &produce(Symbol nonterminal) {
    assert(is_nonterminal(nonterminal) && "expect nonterminal symbol");
    return _productions.at(nonterminal);
  }

  string terminals() { return this->_terminals.to_string(); }

  // 第一项一定是起始符号
  string nonterminals() { return this->_nonterminals.to_string(this->_start); }

  Symbol start() { return this->_start; }

  string to_string() {
    auto ret = string(START_PREFIX) + this->_start + '\n';
    auto terminals = this->terminals();
    auto nonterminals = this->nonterminals();
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

  Symbol alloc_nonterminal() { return this->_nonterminals.alloc(); }

  void alloc_nonterminal(Symbol nonterm) { this->_nonterminals.alloc(nonterm); }

private:
  TerminalSet _terminals;
  NonterminalSet _nonterminals;
  Symbol _start;
  Productions _productions;
};

#endif
