#include "./nfa_from_regexp.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

void test_parser(string_view input, string_view expect) {
  auto actual = Parser(input).parse()->to_string();
  std::cout << actual << std::endl;
  EXPECT_EQ(actual, expect);
}

TEST(PARSER, PARSER1) {
  test_parser("111", "111");
  test_parser("1|1", "(1|1)");
  test_parser("1*", "(1)*");
  test_parser("1***", "(1)*");
  test_parser("(1)", "1");
  test_parser("(0|1)*0.10*", "((0|1))*0.1(0)*");
}

int main(int argc, char *argv[]) {
  printf("Running main() from %s\n", __FILE__);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
