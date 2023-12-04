#include "../../common/CFG.hpp"
#include <iostream>
#include <optional>

void rewrite(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol a_i,
             ContextFreeGrammar::Symbol a_j);

void handle_direct(ContextFreeGrammar &cfg,
                   ContextFreeGrammar::Symbol to_handle);

void handle_epsilon(ContextFreeGrammar &cfg) {
  auto nonterminals = cfg.nonterminals();
  for (auto noterm : nonterminals) {
    auto &rights = cfg.produce(noterm);
    for (int i = 0; i < rights.size(); i++) {
      auto right = rights.at(i);
      if (right.size() == 1 && right.front() == ContextFreeGrammar::EPSILON) {
        rights.erase(rights.begin() + i);
        for (auto to_handle : nonterminals) {
          if (to_handle != noterm) {
            auto &rights = cfg.produce(to_handle);
            auto size = rights.size();
            for (int i = 0; i < size; i++) {
              if (rights.at(i).find(noterm) != string::npos) {
                auto right = rights.at(i);
                auto pos = right.find(noterm);
                while (pos != string::npos) {
                  right.erase(pos, 1);
                  pos = right.find(noterm);
                }
                if (!right.empty()) {
                  rights.push_back(right);
                }
              }
            }
          }
        }
        break;
      }
    }
  }
}

std::optional<ContextFreeGrammar::Symbol> find_epsilon(ContextFreeGrammar &cfg,
                                                       string_view nons) {
  for (auto non : nons) {
    auto &rights = cfg.produce(non);
    for (int i = 0; i < rights.size(); i++) {
      auto right = rights.at(i);
      if (right.size() == 1 && right.front() == ContextFreeGrammar::EPSILON) {
        rights.erase(rights.begin() + i);
        return {non};
      }
    }
  }
  return {};
}

void remove_(ContextFreeGrammar &cfg, string_view nons,
             ContextFreeGrammar::Symbol to_remove) {
  for (auto to_handle : nons) {
  }
}

// 不能推导出环
void left_recursion_kill(ContextFreeGrammar &cfg) {
  auto nonterminals = cfg.nonterminals();
  auto size = nonterminals.size();
  for (int i = 0; i < size; i++) {
    auto a_i = nonterminals.at(i);
    for (int j = 0; j < i; j++) {
      auto a_j = nonterminals.at(j);
      rewrite(cfg, a_i, a_j);
    }
    handle_direct(cfg, a_i);
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
