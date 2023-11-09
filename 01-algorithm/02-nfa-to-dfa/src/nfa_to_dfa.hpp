#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::function;
using std::make_pair;
using std::optional;
using std::set;
using std::sregex_token_iterator;
using std::stack;
using std::stoi;
using std::string_view;
using std::unordered_map;
using std::vector;

struct Util {
  // 用空格符分割不以'\n'结尾的字符串
  static vector<string_view> split_line(string_view line) {
    auto ret = vector<string_view>();
    while (!line.empty()) {
      auto pos = line.find(' ');
      if (pos == string_view::npos) {
        pos = line.size();
      }
      ret.push_back(line.substr(0, pos));
      line = line.substr(pos < line.size() ? pos + 1 : pos);
    }
    return ret;
  }
};

struct Bitset {
  uint64_t content;
  Bitset() : content(0) {}
  Bitset(uint64_t content) : content(content) {}
  Bitset(const Bitset &bitset) : content(bitset.content) {}
  Bitset &operator=(const Bitset &bitset) {
    content = bitset.content;
    return *this;
  }
  Bitset(std::initializer_list<int> list) {
    *this = 0;
    for (auto item : list) {
      this->insert(item);
    }
  }

  Bitset operator|(const Bitset &bitset) const {
    return Bitset(content | bitset.content);
  }
  Bitset operator&(const Bitset &bitset) const {
    return Bitset(content & bitset.content);
  }
  Bitset operator~() const { return Bitset(~content); }
  Bitset operator^(const Bitset &bitset) const {
    return Bitset(content ^ bitset.content);
  }
  Bitset &operator|=(const Bitset &bitset) {
    content |= bitset.content;
    return *this;
  }
  Bitset &operator&=(const Bitset &bitset) {
    content &= bitset.content;
    return *this;
  }
  Bitset &operator^=(const Bitset &bitset) {
    content ^= bitset.content;
    return *this;
  }
  bool operator==(const Bitset &bitset) const {
    return content == bitset.content;
  }
  bool operator!=(const Bitset &bitset) const {
    return content != bitset.content;
  }
  bool operator<(const Bitset &bitset) const {
    return content < bitset.content;
  }
  bool operator>(const Bitset &bitset) const {
    return content > bitset.content;
  }
  bool operator<=(const Bitset &bitset) const {
    return content <= bitset.content;
  }
  bool operator>=(const Bitset &bitset) const {
    return content >= bitset.content;
  }

  bool contains(int i) const {
    assert(i >= 0 && i < 64);
    return (content & (1ULL << i)) != 0;
  }

  bool empty() const { return content == 0; }

  Bitset insert(int i) {
    assert(i >= 0 && i < 64);
    content |= (1ULL << i);
    return *this;
  }

  struct Iterator {
    uint64_t content;
    int index;
    Iterator(uint64_t content, int index) : content(content), index(index) {}
    Iterator &operator++() {
      ++index;
      content >>= 1;
      while (content != 0 && (content & 1) == 0) {
        ++index;
        content >>= 1;
      }
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++*this;
      return tmp;
    }
    bool operator==(const Iterator &it) const { return content == it.content; }
    bool operator!=(const Iterator &it) const { return !(*this == it); }
    int operator*() const { return index; }
  };

  Iterator begin() const {
    uint64_t tmp = content;
    int index = 0;
    while (tmp != 0 && (tmp & 1) == 0) {
      ++index;
      tmp >>= 1;
    }
    return Iterator(tmp, index);
  }

  Iterator end() const { return Iterator(0, 64); }

  static Bitset from_string(string_view str) {
    auto bitset = Bitset();
    assert(str.front() == '{' && str.back() == '}');
    str.remove_prefix(1);
    str.remove_suffix(1);
    while (!str.empty()) {
      auto pos = str.find(',');
      if (pos == string_view::npos) {
        pos = str.size();
      }
      auto num = stoi(std::string(str.substr(0, pos)));
      bitset.insert(num);
      str = str.substr(pos < str.size() ? pos + 1 : pos);
    }
    return bitset;
  }
};

constexpr uint64_t MAX_NFA_STATE = sizeof(uint64_t) * 8;
constexpr char EPSILON = '#';

struct NFA {
  struct State {
    // 对于一个节点和给定的输入符号, 可以转移到的下一个节点的集合的映射
    using Trans = unordered_map<char, Bitset>;
    Trans to;
  };
  // 起始状态的编号
  int start;
  // 终止状态的编号
  int end;
  // 状态集合
  vector<State> states;
  std::string symbols;
  NFA(int start, int end, int total, std::string &&symbols)
      : start(start), end(end), states(total), symbols(symbols) {}

  Bitset epsilon_closure(int id) {
    auto closure = Bitset{id};
    auto s = stack<int>();
    s.push(id);
    while (!s.empty()) {
      auto state = s.top();
      s.pop();
      for (auto i : this->states.at(state).to[EPSILON]) {
        if (!closure.contains(i)) {
          closure.insert(i);
          s.push(i);
        }
      }
    }
    return closure;
  }
  Bitset epsilon_closure(Bitset set) {
    auto closure = set;
    auto s = stack<int>();
    for (auto i : set) {
      s.push(i);
    }
    while (!s.empty()) {
      auto state = s.top();
      s.pop();
      for (auto i : this->states.at(state).to[EPSILON]) {
        if (!closure.contains(i)) {
          closure.insert(i);
          s.push(i);
        }
      }
    }
    return closure;
  }

  Bitset move(int id, char c) { return this->states.at(id).to[c]; }
  Bitset move(Bitset set, char c) {
    auto move_set = Bitset();
    for (auto i : set) {
      move_set |= this->states.at(i).to[c];
    }
    return move_set;
  }

  //<start>
  //<end>
  //<total>
  //<symbols> ...
  //<trans> ...
  //...

  static NFA *from_stdin() {
    char buf[256];
    std::cin.getline(buf, 256);
    auto start = atoi(buf);
    std::cin.getline(buf, 256);
    auto end = atoi(buf);
    std::cin.getline(buf, 256);
    auto total = atoi(buf);
    std::cin.getline(buf, 256);
    auto symbols = std::string(buf);
    auto nfa = new NFA(start, end, total, std::move(symbols));
    for (int i = 0; i < total; i++) {
      std::cin.getline(buf, 256);
      auto tokens = Util::split_line(buf);
      auto to = NFA::State::Trans();
      for (int j = 0; j < tokens.size(); j++) {
        to[symbols[j]] = Bitset::from_string(tokens[j]);
      }
      nfa->states.push_back(NFA::State{to});
    }
    return nfa;
  }
};

struct DFA {
  struct State {
    static constexpr int UNDEFINED = -1;
    int id;
    // 对于dfa state对应nfa state的一个子集
    Bitset nfa_states;
    typedef unordered_map<char, State *> Trans;
    Trans to;
    State() : id(UNDEFINED), nfa_states(), to() {}
    State(Bitset nfa_states) : id(UNDEFINED), nfa_states(nfa_states), to() {}

    bool operator==(const State &state) const {
      return nfa_states == state.nfa_states;
    }
    bool operator!=(const State &state) const { return !(*this == state); }
    bool operator<(const State &state) const {
      return nfa_states < state.nfa_states;
    }
    bool operator>(const State &state) const {
      return nfa_states > state.nfa_states;
    }
    bool operator<=(const State &state) const {
      return nfa_states <= state.nfa_states;
    }
    bool operator>=(const State &state) const {
      return nfa_states >= state.nfa_states;
    }
  };
  State *start;
  set<State *> ends;
  set<State *> states;
  std::string symbols;

  DFA(State *s0, std::string symbols)
      : start(s0), ends(), states(), symbols(symbols) {
    states.insert(s0);
  }

  optional<State *> find_state(function<bool(const State &s)> pred) {
    for (auto state : states) {
      if (pred(*state)) {
        return state;
      }
    }
    return {};
  }

  // 确定终态集
  void set_end_states(int dfa_end) {
    for (auto state : this->states) {
      if (state->nfa_states.contains(dfa_end)) {
        this->ends.insert(state);
      }
    }
  }

  static DFA *from_nfa(NFA &nfa) {
    auto s0 = new State(nfa.epsilon_closure(0));
    auto dfa = new DFA(s0, nfa.symbols);
    auto to_solve = stack<State *>();
    to_solve.push(s0);

    while (!to_solve.empty()) {
      auto state = to_solve.top();
      to_solve.pop();
      for (auto symbol : nfa.symbols) {
        auto move_closure =
            nfa.epsilon_closure(nfa.move(state->nfa_states, symbol));
        if (auto to = dfa->find_state([=](const State &state) -> bool {
              return state.nfa_states == move_closure;
            });
            to.has_value()) {
          state->to.insert(make_pair(symbol, to.value()));
        } else {
          auto new_state = new State(move_closure);
          state->to.insert(make_pair(symbol, new_state));
          to_solve.push(new_state);
          dfa->states.insert(new_state);
        }
      }
    }

    dfa->set_end_states(nfa.end);
    return dfa;
  }
};
