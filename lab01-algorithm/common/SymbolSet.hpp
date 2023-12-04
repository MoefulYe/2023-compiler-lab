#ifndef SYMBOLSET_HPP
#define SYMBOLSET_HPP
#include "./CFG.hpp"
#include <bitset>
using std::bitset;
using std::string;

struct SymbolSet : bitset<128> {
  SymbolSet() : bitset<128>() {}
  SymbolSet(std::initializer_list<ContextFreeGrammar::Symbol> symbols)
      : bitset<128>() {
    for (auto symbol : symbols) {
      this->set(symbol);
    }
  }
  SymbolSet &operator=(const SymbolSet &other) {
    bitset<128>::operator=(other);
    return *this;
  }
  // 重载 | 运算符
  SymbolSet operator|(const SymbolSet &other) const {
    auto ret = SymbolSet();
    ret.bitset<128>::operator=(*this);
    ret |= other;
    return ret;
  }
  // 重载 |= 运算符
  SymbolSet &operator|=(const SymbolSet &other) {
    bitset<128>::operator|=(other);
    return *this;
  }
  string to_string() {
    auto ret = string();
    for (auto c = 0; c < 128; c++) {
      if (this->get(c)) {
        ret += c;
      }
    }
    return ret;
  }
  bool get(char c) { return (*this)[c]; }
  bool contains_epsilon() { return this->get(ContextFreeGrammar::EPSILON); }
  bool contains_and_remove_epsilon() {
    if (this->get(ContextFreeGrammar::EPSILON)) {
      this->reset(ContextFreeGrammar::EPSILON);
      return true;
    } else {
      return false;
    }
  }
  void add_epsilon() { this->set(ContextFreeGrammar::EPSILON); }
  void remove_epsilon() { this->reset(ContextFreeGrammar::EPSILON); }
};

#endif // !#ifndef SYMBOLSET_HPP
