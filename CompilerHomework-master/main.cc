#include<iostream>
#include "task3.tab.hh"

void yy::parser::error(std::string const&err)
{
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
  yy::parser parser;
  parser.parse();
  root.dump();
  return 0;
}
