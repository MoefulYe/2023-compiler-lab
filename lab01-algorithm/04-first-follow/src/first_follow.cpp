#include "./first_follow.h"
#include "../../common/CFG.hpp"

SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
                const ContextFreeGrammar::ProductionRight &right);
SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
                ContextFreeGrammar::Symbol symbol);

SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
                const ContextFreeGrammar::ProductionRight &right) {
  auto ret = SymbolSet();
  auto i = 0;
  for (; i < right.size(); i++) {
    auto symbol = right.at(i);
    auto set = SymbolSet();
    if (ContextFreeGrammar::is_terminal(symbol)) {
      set.set(symbol);
    } else if (ContextFreeGrammar::is_nonterminal(symbol)) {
      if (auto it = memento.find(symbol); it != memento.end()) {
        set = it->second;
      } else {
        set = first(cfg, memento, symbol);
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

SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
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

Map solve_firsts(ContextFreeGrammar &cfg) {
  auto ret = Map();
  auto nonterminals = cfg.nonterminals();
  // 求出所有符号的 first 集
  for (auto symbol : nonterminals) {
    first(cfg, ret, symbol);
  }
  return ret;
}

Map solve_follows(ContextFreeGrammar &cfg, Map &firsts) {
  auto nonterminals = cfg.nonterminals();
  auto start = cfg.start();
  auto follows = Map{{start, {ContextFreeGrammar::END}}};
  for (auto nonterm : nonterminals) {
    follows.insert({nonterm, {}});
  }
  for (;;) {
    auto flag = false;
    for (auto nonterm : nonterminals) {
      auto &follow = follows.at(nonterm);
      for (auto &right : cfg.produce(nonterm)) {
        for (auto i = 0; i < right.size(); i++) {
          if (auto cur = right.at(i); ContextFreeGrammar::is_nonterminal(cur)) {
            auto &follow_i = follows.at(cur);
            auto j = i + 1;
            for (; j < right.size(); j++) {
              auto symbol = right.at(j);
              if (ContextFreeGrammar::is_nonterminal(symbol)) {
                auto first_j = firsts.at(symbol);
                if (first_j.contains_and_remove_epsilon()) {
                  auto new_follow = follow_i | first_j;
                  flag = new_follow != follow_i;
                  follow_i = new_follow;
                } else {
                  auto new_follow = follow_i | first_j;
                  flag = new_follow != follow_i;
                  follow_i = new_follow;
                  break;
                }
              } else {
                auto new_follow = follow_i | SymbolSet{symbol};
                flag = new_follow != follow_i;
                follow_i = new_follow;
                break;
              }
            }
            if (j == right.size()) {
              auto new_follow = follow_i | follow;
              flag = new_follow != follow_i;
              follow_i = new_follow;
            }
          }
        }
      }
    }
    if (!flag) {
      break;
    }
  }
  return follows;
}
