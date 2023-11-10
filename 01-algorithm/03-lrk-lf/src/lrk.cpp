#include "../../common/CFG.hpp"

void rewrite(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol a_i,
             ContextFreeGrammar::Symbol a_j);

void left_recursion_kill(ContextFreeGrammar &cfg) {
  auto nonterminals = cfg.get_terminals();
  auto size = nonterminals.size();
  for (int i = 0; i < size; i++) {
    auto a_i = nonterminals.at(i);
    for (int j = 0; j < i; j++) {
      auto a_j = nonterminals.at(j);
      rewrite(cfg, a_i, a_j);
    }
  }
}
// 消除直接左递归
// 消除后可能会增加一个新的非终结符
void handle_direct(ContextFreeGrammar &cfg,
                   ContextFreeGrammar::Symbol to_handle) {
  auto left_recursion = ContextFreeGrammar::ProductionRights();
  auto no_left_recursion = ContextFreeGrammar::ProductionRights();
  for (auto right : cfg.produce(to_handle)) {
    if (right.front() == to_handle) {
      left_recursion.push_back(right);
    } else {
      no_left_recursion.push_back(right);
    }
  }
  if (!left_recursion.empty()) {
    auto new_nonterm = cfg.alloc_nonterminal();
    for (auto &entry : no_left_recursion) {
      entry.push_back(new_nonterm);
    }
    for (auto &entry : left_recursion) {
      entry.erase(entry.begin());
      entry.push_back(new_nonterm);
    }
    left_recursion.push_back({ContextFreeGrammar::EPSILON});
    cfg.produce(to_handle) = no_left_recursion;
    cfg.produce(new_nonterm) = left_recursion;
  }
}

// 处理Ai -> Ajγ
void rewrite(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol a_i,
             ContextFreeGrammar::Symbol a_j) {
  auto new_rights = ContextFreeGrammar::ProductionRights();
  for (auto right : cfg.produce(a_i)) {
    if (right.front() == a_j) {
      right.erase(right.begin());
      for (auto prefix : cfg.produce(a_j)) {
        prefix.insert(prefix.end(), right.begin(), right.end());
        new_rights.push_back(prefix);
      }
    } else {
      new_rights.push_back(right);
    }
  }
  cfg.produce(a_i) = new_rights;
}
