#include "../include/RegexParser.hpp"

#include <iostream>

bool regex::Parser::match(const std::string& str) {
  pos = 0;
  root = parseExpr();
  if (root == nullptr)
    return false;
  pos = 0;
  return check(root, str);
}

bool regex::Parser::check(pState &node, const std::string &str) {
  bool left, right;
  switch (node->token) {
    case (Token::VerticalBar):
      left = check(node->child[0], str);
      right = check(node->child[1], str);
      if (left || right)
        return true;
      return false;
    break;
    case (Token::Character):
      if (std::dynamic_pointer_cast<StateSym>(node)->sym == str[pos]) {
        ++pos;
        return true;
      }
    return false;
    case (Token::Concat):
      left = check(node->child[0], str);
      right = check(node->child[1], str);
      if (left && right)
        return true;
    return false;
    case (Token::LeftParenthes):
    return check(node->child[0], str);
    case (Token::LeftBracket):
      for (unsigned char sym : std::dynamic_pointer_cast<StateClass>(node)->symbols)
        if (str[pos] == sym) {
          ++pos;
          return true;
        }
    return false;
    default:
      throw "Wrong operation";
  }
}

regex::pState regex::Parser::parseTerm() {
  pState res = nullptr;
  if (pos >= regex_.length())
    return res;
  switch (regex_[pos]) {
    case ('['): res = parseClass(); break;
    case ('\\'):
      res = pState(new StateSym(regex_[++pos]));
      ++pos;
    break;
    case ('.'):
      res = pState(new State(Dot));
      ++pos;
    break;
    case ('('):
      ++pos;
      res = pState(new StateSubRegexp);
      res->child.push_back(parseExpr());
      if (regex_[pos] != ')')
        throw "Expected ')' at pos(" + std::to_string(pos) + ");";
      ++pos;
    break;
    default:
      if (regex_[pos] != '*' &&
          regex_[pos] != '+' &&
          regex_[pos] != '?') {
        res = pState(new StateSym(regex_[pos]));
        ++pos;
      }
  }
  if (pos >= regex_.length())
    return res;
  switch (regex_[pos]) {
    case '*':
      if (res == nullptr)
        throw "Unexpected '*' at pos(" + std::to_string(pos) + ");";
      ++pos;
      res->low = 0;
      res->high = -1;
    break;
    case '+':
      if (res == nullptr)
        throw "Unexpected '*' at pos(" + std::to_string(pos) + ");";
      ++pos;
      res->low = 1;
      res->high = -1;
    break;
    case '?':
      if (res == nullptr)
        throw "Unexpected '*' at pos(" + std::to_string(pos) + ");";
      ++pos;
      res->low = 0;
      res->high = 1;
    break;
    case '{':
      if (res == nullptr)
        throw "Unexpected '*' at pos(" + std::to_string(pos) + ");";
      ++pos;
      if (regex_[pos] != ',' && !std::isdigit(regex_[pos]))
        throw "Unexpected " + std::to_string(regex_[pos]) + " at pos(" + std::to_string(pos) + ");";
      if (regex_[pos] == ',')
        res->low = -1;
      else {
        std::string val;
        while (std::isdigit(regex_[pos]))
          val += regex_[pos++];
        res->low = std::stoll(val);
      }
      if (regex_[pos++] != ',')
        throw "Unexpected " + std::to_string(regex_[pos]) + " at pos(" + std::to_string(pos) + ");";
      if (regex_[pos] != '}' && !std::isdigit(regex_[pos]))
        throw "Unexpected " + std::to_string(regex_[pos]) + " at pos(" + std::to_string(pos) + ");";
      if (regex_[pos] == '}') {
        res->high = -1;
        ++pos;
        return res;
      } else {
        std::string val;
        while (std::isdigit(regex_[pos]))
          val += regex_[pos++];
        res->low = std::stoll(val);
      }
      if (regex_[pos++] != '}')
        throw "Unexpected " + std::to_string(regex_[pos]) + " at pos(" + std::to_string(pos) + ");";
    break;
  }
  return res;
}

regex::pState regex::Parser::parseClass() {
  std::shared_ptr<StateClass> res(new StateClass);
  for (++pos; pos < regex_.length() && regex_[pos] != ']' ;++pos) {
    if (regex_[pos] == '\\')
      res->symbols.push_back(regex_[++pos]);
    else if (regex_[pos] == '-') {
      if (!res->symbols.size())
        res->symbols.push_back('-');
      else {
        ++pos;
        for (unsigned char i = res->symbols.back()+1; i <= regex_[pos]; ++i)
          res->symbols.push_back(i);
      }
    } else
      res->symbols.push_back(regex_[pos]);
  }
  if (regex_[pos] != ']')
    throw "Unexpected end of regular expression; Expected ']'";
  ++pos;
  return res;
}

regex::pState regex::Parser::parseExpr() {
  pState res = parseTerm();
  if (regex_[pos] == ')')
    throw "Unexpected ')' at pos(" + std::to_string(regex_[pos]) + ");";
  pState buff = res;
  pState concat = nullptr;
  while (pos < regex_.length() && regex_[pos] != '|' && regex_[pos] != ')') {
    concat = pState(new StateConcat);
    concat->child.push_back(res);
    concat->child.push_back(parseTerm());
    res = concat;
    // buff->child.push_back(parseTerm());
    // buff = buff->child.back();
  }
  if (pos >= regex_.length())
    return res;
  pState specSym = nullptr;
  while (regex_[pos] == '|') {
    specSym = pState(new StateSeparate);
    ++pos;
    specSym->child.push_back(res);
    specSym->child.push_back(parseTerm());
    res = specSym;
  }

  return res;
}

std::string regex::Parser::print() {
  if (root == nullptr)
    return "";
  return root->print(0);
}
