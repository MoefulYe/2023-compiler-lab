#include "./nfa_to_dfa.hpp"
#include <cstdio>
#include <gtest/gtest.h>

void test_line_split(string_view input, vector<string_view> expect) {
  auto actual = split_line(input);
  EXPECT_EQ(actual.size(), expect.size());
  for (int i = 0; i < actual.size(); i++) {
    EXPECT_EQ(actual[i], expect[i]);
  }
}

void test_bitset_from_string(string_view input, Bitset expect) {
  auto actual = Bitset::from_string(input);
  EXPECT_EQ(actual, expect);
}

int main(int argc, char *argv[]) {
  test_line_split("1 2 3 4 5", {"1", "2", "3", "4", "5"});
  test_line_split("1 2 3 4 5 ", {"1", "2", "3", "4", "5"});
  test_bitset_from_string("{1,2}", Bitset().insert(1).insert(2));
  test_bitset_from_string("{1,2,3}", Bitset().insert(1).insert(2).insert(3));
  test_bitset_from_string("{1,2,3,4}",
                          Bitset().insert(1).insert(2).insert(3).insert(4));
  test_bitset_from_string("{1,2,3,5}",
                          Bitset().insert(1).insert(2).insert(3).insert(5));
}
