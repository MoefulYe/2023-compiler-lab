#include "../../common/CFG.hpp"

using std::map;

struct TrieNode : map<ContextFreeGrammar::Symbol, TrieNode *> {
  TrieNode(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol symbol)
      : map<ContextFreeGrammar::Symbol, TrieNode *>() {
    for (auto &right : cfg.produce(symbol)) {
      auto cur = this;
      for (auto symbol : right) {
        auto res = cur->find(symbol);
        if (res == cur->end()) {
          if (cur != this && cur->size() == 0) {
            cur->insert({ContextFreeGrammar::EPSILON, new TrieNode});
          }
          auto new_child = new TrieNode;
          cur->insert({symbol, new_child});
          cur = new_child;
        } else {
          cur = res->second;
        }
      }
    }
  }

  TrieNode() : map<ContextFreeGrammar::Symbol, TrieNode *>() {}
  ContextFreeGrammar::ProductionRight walk(ContextFreeGrammar &cfg) {
    if (this->size() == 0) {
      return "";
    } else if (this->size() == 1) {
      auto [symbol, child] = *this->cbegin();
      return symbol + child->walk(cfg);
    } else {
      auto new_nonterm = cfg.alloc_nonterminal();
      auto &rights = cfg.produce(new_nonterm);
      for (auto [symbol, child] : *this) {
        rights.push_back(symbol + child->walk(cfg));
      }
      return ContextFreeGrammar::ProductionRight(1, new_nonterm);
    }
  }
};

struct TrieTree {
  ContextFreeGrammar &cfg;
  ContextFreeGrammar::Symbol symbol;
  TrieNode root;
  TrieTree(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol symbol)
      : cfg(cfg), symbol(symbol), root(cfg, symbol){};

  void extract_left_factor() {
    auto &rights = cfg.produce(symbol);
    rights.clear();
    for (auto [symbol, child] : this->root) {
      rights.push_back(symbol + child->walk(cfg));
    }
  }
};

void extract_left_factor(ContextFreeGrammar &cfg) {
  auto non_terms = cfg.get_nonterminals();
  for (auto non_term : non_terms) {
    TrieTree(cfg, non_term).extract_left_factor();
  }
}
