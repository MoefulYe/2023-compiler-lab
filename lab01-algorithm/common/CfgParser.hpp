#ifndef CFG_PARSER_HPP
#define CFG_PARSER_HPP

#include "./CFG.hpp"
#include "./comm.hpp"
#include <cassert>
#include <utility>
#include <vector>

using std::pair;

struct CfgParser {
  string_view input;
  CfgParser(string_view input) : input(input){};
  ContextFreeGrammar parse() {
    auto lines = Util::lines(input);
    assert(lines.size() >= 3);
    auto start = this->extract_start(lines[0]);
    auto nonterminals = this->extract_nonterminals(lines[1]);
    auto cfg = ContextFreeGrammar(start, nonterminals);
    for (int i = 2; i < lines.size(); i++) {
      auto line = lines.at(i);
      auto [left, rights] = this->parse_productions(line);
      auto &products = cfg.produce(left);
      products.insert(products.end(), rights.begin(), rights.end());
    }
    return cfg;
  }

private:
  ContextFreeGrammar::Symbol extract_start(string_view line) {
    constexpr string_view START_WITH = ContextFreeGrammar::START_PREFIX;
    assert(line.substr(0, START_WITH.size()) == START_WITH);
    line = line.substr(START_WITH.size());
    assert(line.size() == 1);
    return line.front();
  }

  string_view extract_nonterminals(string_view line) {
    const string_view START_WITH = ContextFreeGrammar::NONTERMINAL_SET_PREFIX;
    assert(line.substr(0, START_WITH.size()) == START_WITH);
    line = line.substr(START_WITH.size());
    return line;
  }

  pair<ContextFreeGrammar::Symbol, ContextFreeGrammar::ProductionRights>
  parse_productions(string_view line) {
    constexpr auto ARROW = ContextFreeGrammar::ARROW;
    constexpr auto DIVISION = ContextFreeGrammar::DIVISION;
    assert(line.size() > 1 + ARROW.size());
    assert(line.substr(1, ARROW.size()) == ARROW);
    auto left = line.front();
    line = line.substr(1 + ARROW.size());
    auto rights = ContextFreeGrammar::ProductionRights{};
    while (!line.empty()) {
      auto pos = line.find(DIVISION);
      if (pos == string_view::npos) {
        pos = line.size();
      }
      rights.push_back(string(line.substr(0, pos)));
      line = line.substr(pos < line.size() ? pos + 1 : pos);
    }
    return {left, rights};
  }
};

#endif // !CFG_PARSER_HPP
