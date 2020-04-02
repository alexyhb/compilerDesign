#include<iostream>
#include "binary.tab.hh"
extern unsigned int total;

void yy::parser::error(std::string const&err)
{
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int main(int argc, char **argv)
{
yy::parser parser;
  if(!parser.parse())
    std::cout << "It's a bingo! " << total << std::endl;
  return 0;
}