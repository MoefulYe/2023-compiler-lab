#ifndef FIRST_H
#define FIRST_H
#include "../../common/CFG.hpp"
#include "../../common/SymbolSet.hpp"

using Memento = unordered_map<ContextFreeGrammar::Symbol, SymbolSet>;

SymbolSet first(ContextFreeGrammar &cfg, Memento &memento,
                const ContextFreeGrammar::ProductionRight &right);
SymbolSet first(ContextFreeGrammar &cfg, Memento &memento,
                ContextFreeGrammar::Symbol symbol);
#endif // !#ifndef FIRST_H
