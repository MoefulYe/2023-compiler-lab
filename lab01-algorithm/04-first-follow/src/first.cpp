#include "./first.h"
#include "../../common/CFG.hpp"
SymbolSet first(ContextFreeGrammar &cfg, Memento &memento,
                const ContextFreeGrammar::ProductionRight &right) {
  auto ret = SymbolSet();
  auto i = 0;
  for (auto symbol = right[i]; i < right.size(); i++) {
    auto set = SymbolSet();
    if (ContextFreeGrammar::is_terminal(symbol)) {
      set.set(symbol);
    } else if (ContextFreeGrammar::is_nonterminal(symbol)) {
      if (auto it = memento.find(symbol); it != memento.end()) {
        set = it->second;
      } else {
        first(cfg, memento, symbol);
      }
    } else if (ContextFreeGrammar::is_epsilon(symbol)) {
      set.add_epsilon();
    }
    if (set.contains_and_remove_epsilon()) {
      ret |= set;
    } else {
      ret |= set;
      break;
    }
  }
  if (i == right.size()) {
    ret.add_epsilon();
  }
  return ret;
}

SymbolSet first(ContextFreeGrammar &cfg, Memento &memento,
                ContextFreeGrammar::Symbol symbol) {
  if (auto it = memento.find(symbol); it != memento.end()) {
    return it->second;
  } else {
    auto ret = SymbolSet();
    for (const auto &right : cfg.produce(symbol)) {
      ret |= first(cfg, memento, right);
    }
    memento.insert({symbol, ret});
    return ret;
  }
}
