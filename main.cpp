#include <iostream>
#include "include/RegexParser.hpp"

int main() {
  regex::Parser parser;
  // parser.regex() = "ab";
  // parser.regex() = "(ab)*";
  // parser.regex() = "a|b";
  // parser.regex() = "a.?";
  // parser.regex() = "a*\\+";
  parser.regex() = "(a|b)*";
  std::cout << parser.match("c") << std::endl;
  std::cout << parser.print() << std::endl;
  return 0;
}
