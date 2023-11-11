#include "./nfa_to_dfa.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <string_view>
#include <vector>
using testing::Test;

struct LineSplitTester : public Test {
  void test(string_view input, vector<string_view> expect) {
    auto actual = Util::split_line(input);
    EXPECT_EQ(actual.size(), expect.size());
    for (int i = 0; i < actual.size(); i++) {
      EXPECT_EQ(actual[i], expect[i]);
    }
  }
};
TEST_F(LineSplitTester, LineSplit) {
  test("1 2 3 4", {"1", "2", "3", "4"});
  test("1 2 3 ", {"1", "2", "3"});
  test("111 22 3333", {"111", "22", "3333"});
}

struct Srt2BitsetTester : public Test {
  void test(string_view input, Bitset expect) {
    auto actual = Bitset::from_string(input);
    EXPECT_EQ(actual, expect);
  }
};
TEST_F(Srt2BitsetTester, Str2Bitset) {
  test("{1,2,3}", Bitset{1, 2, 3});
  test("{1,2,3,}", Bitset{1, 2, 3});
  test("{1,6,9,10}", Bitset{1, 6, 9, 10});
}

struct BitsetIterTest : public Test {
  void test(Bitset set, vector<int> nums) {
    auto i = 0;
    for (auto num : set) {
      EXPECT_EQ(num, nums[i++]);
    }
  }
};
TEST_F(BitsetIterTest, IterTest) {
  test({1, 2, 3}, {1, 2, 3});
  test({1, 2, 3, 4}, {1, 2, 3, 4});
  test({1, 5, 3, 4}, {1, 3, 4, 5});
}

struct NFATester : public Test {
  NFA *nfa;
  void SetUp() override {
    // 测试例来自https://zhuanlan.zhihu.com/p/31158595
    using Trans = NFA::State::Trans;
    auto start = 0;
    auto end = 9;
    auto total = 10;
    auto symbols = "ab#";
    nfa = new NFA(start, end, total, symbols);
    nfa->states[0].to = Trans{{'a', {1}}};
    nfa->states[1].to = Trans{{'#', {2}}};
    nfa->states[2].to = Trans{{'#', {3, 9}}};
    nfa->states[3].to = Trans{{'#', {4, 6}}};
    nfa->states[4].to = Trans{{'b', {5}}};
    nfa->states[5].to = Trans{{'#', {8}}};
    nfa->states[6].to = Trans{{'c', {7}}};
    nfa->states[7].to = Trans{{'#', {8}}};
    nfa->states[8].to = Trans{{'#', {3, 9}}};
    nfa->states[9].to = Trans{};
  }
  void test_closure() {
    auto s0 = nfa->epsilon_closure(nfa->start);
    EXPECT_EQ(s0, Bitset({0}));
  }

  void move() {
    auto s0 = nfa->epsilon_closure(nfa->start);
    auto s1 = nfa->epsilon_closure(nfa->move(s0, 'a'));
    EXPECT_EQ(s1, Bitset({1, 2, 3, 4, 6, 9}));
    auto s2 = nfa->epsilon_closure(nfa->move(s1, 'b'));
    EXPECT_EQ(s2, Bitset({5, 8, 9, 3, 4, 6}));
  }

  void to_dfa() { auto dfa = DFA::from_nfa(*nfa); }
};

TEST_F(NFATester, TestNFA) {
  test_closure();
  move();
  to_dfa();
}

int main(int argc, char *argv[]) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
