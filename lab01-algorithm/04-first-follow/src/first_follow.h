#ifndef FIRST_H
#define FIRST_H
#include "../../common/CFG.hpp"
#include "../../common/SymbolSet.hpp"
using Map = map<ContextFreeGrammar::Symbol, SymbolSet>;
Map solve_firsts(ContextFreeGrammar &cfg);
Map solve_follows(ContextFreeGrammar &cfg, Map &firsts);
#endif // !#ifndef FIRST_H
