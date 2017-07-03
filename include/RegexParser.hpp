#pragma once

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <map>


namespace regex {

enum Token {
  None,           //''
  RightParenthes, //')'
  LeftParenthes,  //'('
  RightBracket,   //']'
  LeftBracket,    //'['
  RightBrace,     //'}'
  LeftBrace,      //'{'
  BackSlash,      //'\'
  Dot,            //'.' - any single character (not \r, \n)
  Star,           //'*' - zero or more of the preciding character
  Question,       //'?' - zero or one of the preciding character
  Plus,           //'+' - one or more of the preciding character
  VerticalBar,    //'|' - separates two alternatives
  Circumflex,     //'^' - from beginning
  Dollar,         //'$' - at the end
  Character,      //[a-zA-Z0-9]
};

static std::map<Token, char> Tokens {
  {RightParenthes, ')'},
  {LeftParenthes, '('},
  {RightBracket, ']'},
  {LeftBracket, '['},
  {RightBrace, '}'},
  {LeftBrace, '{'},
  {BackSlash, '\\'},
  {Dot, '.'},
  {Star, '*'},
  {Question, '?'},
  {Plus, '+'},
  {VerticalBar, '|'},
  {Circumflex, '^'},
  {Dollar, '$'},
};

struct State;
typedef std::shared_ptr<State> pState;

const int DEEP_STEP = 3;
const char DEEP_CHAR = ' ';

struct State {
  State (Token token = None) : token(token) {};
  Token token;
  size_t pos;
  std::vector<pState> child;
  int low = 1, high = 1;
  virtual std::string print(int deep) {
    std::ostringstream sstream;
    sstream << std::string(deep * DEEP_STEP, DEEP_CHAR) << Tokens[token] << '\n'
            << " {"
            << ((low < 0) ? "-inf" : std::to_string(low)) << ", "
            << ((high < 0) ? "inf" : std::to_string(high)) << "}\n";
    for (int i = 0; i < child.size(); ++i)
      sstream << child[i]->print(deep+1) << '\n';
    return sstream.str();
  };
};

struct StateSeparate : public State {
  StateSeparate () : State(VerticalBar) {};
  std::string print(int deep) override {
    std::ostringstream sstream;
    for (int i = 0; i < child.size()/2; ++i)
      sstream << child[i]->print(deep+1) << '\n';
    sstream << std::string(deep * DEEP_STEP, DEEP_CHAR) << '|' << std::endl;
    for (int i = child.size()/2; i < child.size(); ++i)
      sstream << child[i]->print(deep+1) << '\n';
    return sstream.str();
  };
};

struct StateSubRegexp : public State {
  StateSubRegexp () : State(LeftParenthes) {};
  std::string print(int deep) override {
    std::ostringstream sstream;
    sstream << std::string(deep * DEEP_STEP, DEEP_CHAR) << "()\n"
            << " {"
            << ((low < 0) ? "-inf" : std::to_string(low)) << ", "
            << ((high < 0) ? "inf" : std::to_string(high)) << "}\n";
    for (int i = 0; i < child.size(); ++i)
      sstream << child[i]->print(deep+1) << '\n';
    return sstream.str();
  };
};

struct StateSym : public State {
  StateSym (unsigned char sym) : State(Character), sym(sym) {};
  unsigned char sym;
  std::string print(int deep) override {
    std::ostringstream sstream;
    sstream << std::string(deep * DEEP_STEP, DEEP_CHAR) << sym
            << " {" << low << ", " << high << "}\n";
    for (int i = 0; i < child.size(); ++i)
      sstream << child[i]->print(deep+1);
    return sstream.str();
  };
};

struct StateClass : public State {
  StateClass () : State(LeftBracket) {};
  std::vector<unsigned char> symbols;
  std::string print(int deep) override {
    std::ostringstream sstream;
    sstream << std::string(deep * DEEP_STEP, DEEP_CHAR) << '[';
    for (auto elem : symbols)
      sstream << elem;
    sstream << ']' << std::endl;
    for (int i = 0; i < child.size(); ++i)
      sstream << child[i]->print(deep+1);
    return sstream.str();
  };
};

class Parser {
public:
  Parser () {};
  Parser (const std::string& regex) : regex_(regex) {};
  std::string& regex() {return regex_;}
  void parse(const std::string& str);
  std::string print();
private:
  pState parseTerm();
  pState parseExpr();
  pState parseClass();

  pState genToken(unsigned char sym);

  std::string regex_;
  pState root;
  size_t pos;
};

}
