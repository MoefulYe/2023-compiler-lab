#include "../../common/CFG.hpp"
#include "../../common/SymbolAllocator.hpp"
#include <unordered_map>
#include <vector>

using std::unordered_map;

struct TrieNode {
  unordered_map<Symbol *, TrieNode *> children;
  int childrens() { return children.size(); }
  void insert(Symbol *symbol, TrieNode *node) { children[symbol] = node; }
  TrieNode *via(Symbol *symbol) { return children[symbol]; }
  TrieNode() {}
  TrieNode(Nonterminal *root, SymbolAllcator &allocator) {
    for (auto &production : root->productions) {
      auto cur = this;
      for (auto symbol : production) {
        if (cur->children.find(symbol) == cur->children.end()) {
          if (cur->childrens() == 0 && cur != this) {
            this->insert(allocator.get_or_alloc_epsilon(), new TrieNode);
          }
          cur->insert(symbol, new TrieNode);
        }
        cur = cur->via(symbol);
      }
    }
  }
  vector<Nonterminal *> to_productions(SymbolAllcator &allocator) {
    auto ret = vector<Nonterminal *>();
    for (auto &[symbol, node] : this->children) {
      if (symbol == allocator.get_or_alloc_epsilon()) {
        ret.push_back(nullptr);
      } else {
        auto productions = node->to_productions(allocator);
        for (auto production : productions) {
          production->insert(production->begin(), symbol);
        }
        ret.insert(ret.end(), productions.begin(), productions.end());
      }
    }
    return ret;
  }
};
