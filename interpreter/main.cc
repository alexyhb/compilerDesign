#include <iostream>
#include <unordered_map>
#include <fstream>
#include "lua.tab.hh"

void yy::parser::error(std::string const&err)
{
    std::cout << "It's one of the bad ones... " << err << std::endl;
}

std::unordered_map<std::string, Node> Node::storage = {};

int main(int argc, char **argv)
{
    FILE* target = std::fopen(argv[1], "r");
    if(target == NULL)
        yyin = stdin;
    else
        yyin = target;
    yy::parser parser;
    parser.parse();
    // root.dump();
    // dump to dot form text file
    std::ofstream SaveFile("parse.dot");
    SaveFile << root.dump_to_dot();
    SaveFile.close();
    root.eval();
}