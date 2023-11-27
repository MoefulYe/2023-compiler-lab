#include "../../common/CFG.hpp"

using std::map;

struct TrieNode : map<ContextFreeGrammar::Symbol, TrieNode *> {
  TrieNode(ContextFreeGrammar &cfg, ContextFreeGrammar::Symbol symbol)
      : map<ContextFreeGrammar::Symbol, TrieNode *>() {
    for (auto &right : cfg.produce(symbol)) {
      auto cur = this;
      auto it = right.begin();
      for (;;) {
        // 迭代直到遍历完右部或者在树上找不到对应的节点
        if (it == right.end()) {
          cur->insert({ContextFreeGrammar::EPSILON, new TrieNode});
          break;
        } else if (auto entry = cur->find(*it); entry == cur->end()) {
          if (cur->size() == 0 && cur != this) {
            cur->insert({ContextFreeGrammar::EPSILON, new TrieNode});
          }
          break;
        } else {
          cur = entry->second;
          it++;
        }
      }
      while (it != right.end()) {
        auto new_child = new TrieNode;
        cur->insert({*it, new_child});
        cur = new_child;
        it++;
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
      return ContextFreeGrammar::ProductionRight{new_nonterm};
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
  auto non_terms = cfg.nonterminals();
  for (auto non_term : non_terms) {
    TrieTree(cfg, non_term).extract_left_factor();
  }
}
