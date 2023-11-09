#include "../../common/CFG.hpp"

optional<Nonterminal *> handle_direct(Nonterminal *to_handle);
void rewrite(Nonterminal *to_handle, Nonterminal *prefix);

void left_recursion_kill(ContextFreeGrammar &cfg) {
  auto &nonterminals = cfg.nonterminals;
  auto size = nonterminals.size();
  for (int i = 0; i < size; i++) {
    auto a_i = nonterminals.at(i);
    for (int j = 0; j < i; j++) {
      auto a_j = nonterminals.at(j);
      rewrite(a_i, a_j);
    }
    if (auto new_nonterminal = handle_direct(a_i);
        new_nonterminal.has_value()) {
      nonterminals.push_back(new_nonterminal.value());
    }
  }
}

// 消除直接左递归
// 消除后可能会增加一个新的非终结符
optional<Nonterminal *> handle_direct(Nonterminal *to_handle) {
  auto left_recursion_productions = vector<Nonterminal::Production>();
  auto no_left_recursion_productions = vector<Nonterminal::Production>();
  for (auto production : to_handle->productions) {
    if (production.at(0) == to_handle) {
      left_recursion_productions.push_back(production);
    } else {
      no_left_recursion_productions.push_back(production);
    }
  }
  if (left_recursion_productions.empty()) {
    return {};
  } else {
    auto new_nonterminal = new Nonterminal(to_handle->symbol + "'");
    for (auto &production : no_left_recursion_productions) {
      production.push_back(new_nonterminal);
    }
    for (auto &production : left_recursion_productions) {
      production.erase(production.begin());
      production.push_back(new_nonterminal);
    }
    left_recursion_productions.push_back({new Epsilon});
    to_handle->productions = no_left_recursion_productions;
    new_nonterminal->productions = left_recursion_productions;
    return {new_nonterminal};
  }
}

// 处理Ai -> Ajγ
void rewrite(Nonterminal *a_i, Nonterminal *a_j) {
  auto new_productions = vector<Nonterminal::Production>();
  for (auto production : a_i->productions) {
    if (production.at(0) == a_j) {
      production.erase(production.begin());
      for (auto prefix_production : a_j->productions) {
        auto new_production = prefix_production;
        new_production.insert(new_production.end(), production.begin(),
                              production.end());
        new_productions.push_back(new_production);
      }
    } else {
      new_productions.push_back(production);
    }
  }
  a_i->productions = new_productions;
}
