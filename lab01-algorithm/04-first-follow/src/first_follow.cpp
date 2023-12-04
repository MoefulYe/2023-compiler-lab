#include "./first_follow.h"
#include "../../common/CFG.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

// 对于有左递归或者间接左递归的文法会爆栈
// SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
//                 const ContextFreeGrammar::ProductionRight &right);
// SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
//                 ContextFreeGrammar::Symbol symbol);
//
// SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
//                 const ContextFreeGrammar::ProductionRight &right) {
//   auto ret = SymbolSet();
//   auto i = 0;
//   for (; i < right.size(); i++) {
//     auto symbol = right.at(i);
//     auto set = SymbolSet();
//     if (ContextFreeGrammar::is_terminal(symbol)) {
//       set.set(symbol);
//     } else if (ContextFreeGrammar::is_nonterminal(symbol)) {
//       if (auto it = memento.find(symbol); it != memento.end()) {
//         set = it->second;
//       } else {
//         set = first(cfg, memento, symbol);
//       }
//     } else if (ContextFreeGrammar::is_epsilon(symbol)) {
//       set.add_epsilon();
//     }
//     if (set.contains_and_remove_epsilon()) {
//       ret |= set;
//     } else {
//       ret |= set;
//       break;
//     }
//   }
//   if (i == right.size()) {
//     ret.add_epsilon();
//   }
//   return ret;
// }
//
// SymbolSet first(ContextFreeGrammar &cfg, Map &memento,
//                 ContextFreeGrammar::Symbol symbol) {
//   if (auto it = memento.find(symbol); it != memento.end()) {
//     return it->second;
//   } else {
//     auto ret = SymbolSet();
//     for (const auto &right : cfg.produce(symbol)) {
//       ret |= first(cfg, memento, right);
//     }
//     memento.insert({symbol, ret});
//     return ret;
//   }
// }

// bool epsilonable(ContextFreeGrammar &cfg,
//                  ContextFreeGrammar::ProductionRight &right,
//                  std::unordered_set<ContextFreeGrammar::Symbol>
//                  &has_visited);
// bool epsilonable(ContextFreeGrammar &cfg,
//                  ContextFreeGrammar::ProductionRight &right,
//                  std::unordered_set<ContextFreeGrammar::Symbol>
//                  &has_visited);
//
// bool epsilonable(ContextFreeGrammar &cfg,
//                  ContextFreeGrammar::ProductionRight &right,
//                  std::unordered_set<ContextFreeGrammar::Symbol> &has_visited)
//                  {
//   for (auto symbol : right) {
//     if (cfg.is_nonterminal(symbol)) {
//       if (has_visited.find(symbol) == has_visited.end()) {
//         if (!epsilonable(cfg, right, has_visited)) {
//           return false;
//         }
//       } else {
//         // 如果找的到如果那么即使epsilon会通过其他的路径找到所以直接返回false
//         return false;
//       }
//     } else if (cfg.is_terminal(symbol)) {
//       return false;
//     }
//   }
//   return true;
// }
//
// bool epsilonable(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol symbol,
//                  std::unordered_set<ContextFreeGrammar::Symbol> &has_visited)
//                  {
//   if (cfg.is_terminal(symbol)) {
//     return false;
//   } else if (cfg.is_epsilon(symbol)) {
//     return true;
//   } else {
//     if (has_visited.find(symbol) != has_visited.end()) {
//       return false;
//     } else {
//       has_visited.insert(symbol);
//       for (auto &right : cfg.produce(symbol)) {
//         if (epsilonable(cfg, right, has_visited)) {
//           has_visited.erase(symbol);
//           return true;
//         }
//       }
//       has_visited.erase(symbol);
//       return false;
//     }
//   }
// }
//
// void first(ContextFreeGrammar &cfg, ContextFreeGrammar::ProductionRight
// &right,
//            SymbolSet &ret,
//            std::unordered_set<ContextFreeGrammar::Symbol> &has_visited) {
//   auto size = right.size();
//   for (int i = 0; i < size; i++) {
//     auto symbol = right.at(i);
//     if (cfg.is_nonterminal(symbol)) {
//       // 外层还没有处理过
//       if (has_visited.find(symbol) == has_visited.end()) {
//         has_visited.insert(symbol);
//         for (auto &right : cfg.produce(symbol)) {
//           first(cfg, right, ret, has_visited);
//         }
//         has_visited.erase(symbol);
//       }
//     } else if (cfg.is_terminal(symbol)) {
//       ret |= {symbol};
//     }
//     ret.remove_epsilon();
//     if (!epsilonable(cfg, symbol, has_visited)) {
//       // 如果不可推出epsilon那么就不用继续了
//       break;
//     } else {
//       if (size - 1 == i) {
//         ret.add_epsilon();
//       }
//     }
//   }
// }
//
// SymbolSet first(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol symbol) {
//   auto ret = SymbolSet();
//   for (auto &right : cfg.produce(symbol)) {
//     auto has_visited =
//     std::unordered_set<ContextFreeGrammar::Symbol>{symbol}; first(cfg, right,
//     ret, has_visited);
//   }
//   return ret;
// }

SymbolSet first(ContextFreeGrammar &cfg, Map &firsts,
                const ContextFreeGrammar::ProductionRight &right);
bool first(ContextFreeGrammar &cfg, Map &firsts,
           ContextFreeGrammar::Symbol symbol);

SymbolSet first(ContextFreeGrammar &cfg, Map &firsts,
                const ContextFreeGrammar::ProductionRight &right) {
  auto ret = SymbolSet();
  auto i = 0;
  for (; i < right.size(); i++) {
    auto symbol = right.at(i);
    auto set = SymbolSet();
    if (ContextFreeGrammar::is_terminal(symbol)) {
      set.set(symbol);
    } else if (ContextFreeGrammar::is_nonterminal(symbol)) {
      set = firsts.at(symbol);
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

bool first(ContextFreeGrammar &cfg, Map &firsts,
           ContextFreeGrammar::Symbol symbol) {
  auto &old = firsts.at(symbol);
  auto new_ = SymbolSet();
  for (const auto &right : cfg.produce(symbol)) {
    new_ |= first(cfg, firsts, right);
  }
  auto ret = new_ != old;
  old = new_;
  return ret;
}

Map solve_firsts(ContextFreeGrammar &cfg) {
  auto ret = Map();
  auto nonterminals = cfg.nonterminals();
  for (auto symbol : nonterminals) {
    ret.insert({symbol, {}});
  }
  for (;;) {
    auto flag = false;
    for (auto symbol : nonterminals) {
      flag |= first(cfg, ret, symbol);
    }
    if (!flag) {
      break;
    }
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
                  flag |= new_follow != follow_i;
                  follow_i = new_follow;
                } else {
                  auto new_follow = follow_i | first_j;
                  flag |= new_follow != follow_i;
                  follow_i = new_follow;
                  break;
                }
              } else if (ContextFreeGrammar::is_epsilon(symbol)) {
                continue;
              } else {
                auto new_follow = follow_i | SymbolSet{symbol};
                flag |= new_follow != follow_i;
                follow_i = new_follow;
                break;
              }
            }
            if (j == right.size()) {
              auto new_follow = follow_i | follow;
              flag |= new_follow != follow_i;
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
