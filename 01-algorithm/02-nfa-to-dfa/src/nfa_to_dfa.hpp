#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
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
using std::regex;
using std::set;
using std::sregex_token_iterator;
using std::stack;
using std::stoi;
using std::string_view;
using std::unordered_map;
using std::vector;

struct Bitset {
  uint64_t content;
  Bitset() : content(0) {}
  Bitset(uint64_t content) : content(content) {}
  Bitset(const Bitset &bitmap) : content(bitmap.content) {}
  Bitset &operator=(const Bitset &bitmap) {
    content = bitmap.content;
    return *this;
  }

  Bitset operator|(const Bitset &bitmap) const {
    return Bitset(content | bitmap.content);
  }
  Bitset operator&(const Bitset &bitmap) const {
    return Bitset(content & bitmap.content);
  }
  Bitset operator~() const { return Bitset(~content); }
  Bitset operator^(const Bitset &bitmap) const {
    return Bitset(content ^ bitmap.content);
  }
  Bitset &operator|=(const Bitset &bitmap) {
    content |= bitmap.content;
    return *this;
  }
  Bitset &operator&=(const Bitset &bitmap) {
    content &= bitmap.content;
    return *this;
  }
  Bitset &operator^=(const Bitset &bitmap) {
    content ^= bitmap.content;
    return *this;
  }
  bool operator==(const Bitset &bitmap) const {
    return content == bitmap.content;
  }
  bool operator!=(const Bitset &bitmap) const {
    return content != bitmap.content;
  }
  bool operator<(const Bitset &bitmap) const {
    return content < bitmap.content;
  }
  bool operator>(const Bitset &bitmap) const {
    return content > bitmap.content;
  }
  bool operator<=(const Bitset &bitmap) const {
    return content <= bitmap.content;
  }
  bool operator>=(const Bitset &bitmap) const {
    return content >= bitmap.content;
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
    str = str.substr(1, str.size() - 2);
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
    typedef unordered_map<char, Bitset> Trans;
    Trans to;
  };
  // 起始状态的编号
  int start;
  // 终止状态的编号
  int end;
  // 状态集合
  vector<State> states;
  NFA(int start, int end, int total) : start(start), end(end), states(total) {}
};

// 为nfa状态编号和nfa状态位向量集合, 特化该模板
template <typename T> Bitset epsilon_closure(const NFA &nfa, T t) {
  assert(false);
}

template <> Bitset epsilon_closure(const NFA &nfa, int id) {
  auto closure = Bitset().insert(id);
  auto s = stack<int>();
  s.push(id);
  while (!s.empty()) {
    auto state = s.top();
    s.pop();
    for (auto i : nfa.states.at(state).to.at(EPSILON)) {
      if (!closure.contains(i)) {
        closure.insert(i);
        s.push(i);
      }
    }
  }
  return closure;
}

template <> Bitset epsilon_closure(const NFA &nfa, Bitset set) {
  auto closure = set;
  auto s = stack<int>();
  for (auto i : set) {
    s.push(i);
  }
  while (!s.empty()) {
    auto state = s.top();
    s.pop();
    for (auto i : nfa.states.at(state).to.at(EPSILON)) {
      if (!closure.contains(i)) {
        closure.insert(i);
        s.push(i);
      }
    }
  }
  return closure;
}

template <typename T> Bitset move(const NFA &nfa, T t, char c) {
  assert(false);
}

template <> Bitset move(const NFA &nfa, int id, char c) {
  return nfa.states.at(id).to.at(c);
}

template <> Bitset move(const NFA &nfa, Bitset set, char c) {
  auto move_set = Bitset();
  for (auto i : set) {
    move_set |= nfa.states.at(i).to.at(c);
  }
  return move_set;
}

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

  DFA(State *s0) : start(s0), ends(), states() { states.insert(s0); }

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

  static DFA *from_nfa(const NFA &nfa, std::string &symbols) {
    auto s0 = new State(epsilon_closure(nfa, nfa.start));
    auto dfa = new DFA(s0);
    auto to_solve = stack<State *>();
    to_solve.push(s0);

    while (!to_solve.empty()) {
      auto state = to_solve.top();
      to_solve.pop();
      for (auto symbol : symbols) {
        auto move_closure =
            epsilon_closure(nfa, move(nfa, state->nfa_states, symbol));
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

// 用空格符分割不以'\n'结尾的字符串
vector<string_view> split_line(string_view line) {
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
