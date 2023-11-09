#include "../../common/CFG.hpp"

using std::unordered_map;

struct Tree {
  struct Node {
    unordered_map<Symbol *, Node *> child;
  };
  Node node;
  Tree(Nonterminal *nonterminal) {
    for (auto &production : nonterminal->productions) {
      auto current = &this->node;
      for (auto symbol : production) {
        if (current->child.find(symbol) == current->child.end()) {
          // 如果没有找到, 就创建一个新的节点
          current->child[symbol] = new Node;
        }
        current = current->child[symbol];
      }
    }
  }
};
