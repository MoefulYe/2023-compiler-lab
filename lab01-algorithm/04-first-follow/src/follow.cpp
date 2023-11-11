#include "../../common/CFG.hpp"
#include "../../common/SymbolSet.hpp"
#include "./first.h"

using Map = unordered_map<ContextFreeGrammar::Symbol, SymbolSet>;

Map follow(ContextFreeGrammar &cfg) {
  auto firsts = Map();
  auto terminals = cfg.terminals();
  auto nonterminals = cfg.nonterminals();
  auto start = cfg.start();
  // 求出所有符号的 first 集
  for (auto symbol : nonterminals) {
    first(cfg, firsts, symbol);
  }
  for (auto symbol : terminals) {
    firsts.insert({symbol, {symbol}});
  }

  auto follows = Map({cfg.start(), ContextFreeGrammar::END});
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
              auto first_j = firsts.at(right.at(j));
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
