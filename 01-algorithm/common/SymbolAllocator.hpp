#ifndef SYMBOL_ALLOCATOR_HPP
#define SYMBOL_ALLOCATOR_HPP

#include "CFG.hpp"
#include <array>
#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>

struct SymbolAllcator {
  std::array<Terminal *, 26> terminal_alloc_table;
  std::array<Nonterminal *, 26> nonterminal_alloc_table;
  Epsilon *epsilon;
  SymbolAllcator()
      : epsilon(nullptr), nonterminal_alloc_table(), terminal_alloc_table() {}

  Epsilon *get_epsilon() {
    assert(this->epsilon != nullptr);
    return this->epsilon;
  }
  Terminal *get_terminal(char symbol) {
    assert(this->terminal_alloc_table.at(symbol - 'a') != nullptr);
    return this->terminal_alloc_table.at(symbol - 'a');
  }
  Nonterminal *get_nonerminal(char symbol) {
    assert(this->nonterminal_alloc_table.at(symbol - 'A') != nullptr);
    return this->nonterminal_alloc_table.at(symbol - 'A');
  }
  Epsilon *get_or_alloc_epsilon() {
    if (this->epsilon == nullptr) {
      this->epsilon = new Epsilon;
    }
    return this->epsilon;
  }
  Terminal *get_or_alloc_terminal(char symbol) {
    if (this->terminal_alloc_table.at(symbol - 'a') == nullptr) {
      this->terminal_alloc_table.at(symbol - 'a') = new Terminal(symbol);
    }
    return this->terminal_alloc_table.at(symbol - 'a');
  }
  Nonterminal *get_or_alloc_nonterminal(char symbol) {
    if (this->nonterminal_alloc_table.at(symbol - 'A') == nullptr) {
      this->nonterminal_alloc_table.at(symbol - 'A') = new Nonterminal(symbol);
    }
    return this->nonterminal_alloc_table.at(symbol - 'A');
  }
  Nonterminal *alloc_nonterminal() {
    for (int i = 0; i < 26; i++) {
      if (this->nonterminal_alloc_table.at(i) == nullptr) {
        this->nonterminal_alloc_table.at(i) = new Nonterminal('A' + i);
        return this->nonterminal_alloc_table.at(i);
      }
    }
    assert(false);
  }
  Nonterminal *alloc_nonterminal(char symbol) {
    assert(this->nonterminal_alloc_table.at(symbol - 'A') == nullptr);
    this->nonterminal_alloc_table.at(symbol - 'A') = new Nonterminal(symbol);
    return this->nonterminal_alloc_table.at(symbol - 'A');
  }
};

#endif // !#ifndef SYMBOL_ALLOCATOR_HPP
