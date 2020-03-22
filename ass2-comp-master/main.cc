#include <iostream>
#include <fstream>
#include <set>
#include "lua.tab.hh"

void yy::parser::error(std::string const&err) {
  std::cout << "It's one of the bad ones... " << err << std::endl;
}

int BBlock::nCounter = 0;
int Exp::nameCounter = 0;
list<Procedure*> Procedure::procStorage = {};
set<string> Global::all_variables = {};
unordered_map<string, int> Global::all_tables = {};

string wrap_dot(list<Procedure*> s) {
    string dot = "digraph G {\nnode[shape=box]\n";
    for(auto i : s) dot += i->to_dot();
    dot += "}";
    return dot;
}

string wrap_cc(list<Procedure*> s) {
    string cc = "#include <stdio.h>\n#include <math.h>\n\n";
    // cc += "double ";
    // for (auto iter = Global::all_variables.cbegin(); iter != Global::all_variables.cend(); iter++) {
    //     cc = cc + (*iter);
    //     set <string>::iterator iter2 = iter;
    //     ++iter2;
    //     if(iter2 != Global::all_variables.cend()) cc += ", ";
    // }
    // cc += ";\n";
    // for (auto iter = Global::all_tables.cbegin(); iter != Global::all_tables.cend(); iter++)
    //     cc = cc + "double " + iter->first + "[" + to_string(iter->second) + "];\n";
    // cc += "\n";
    for(auto i : s) cc += i->to_c() + "\n";
    return cc;
}

int main(int argc, char **argv)
{
    FILE* target = std::fopen(argv[1], "r");
    if(target == NULL) yyin = stdin;
    else yyin = target;
    
    yy::parser parser;
    parser.parse();

    BBlock *cfg = root -> convert(new BBlock());
    Procedure *proc = new Procedure("main", "", cfg);
    Procedure::procStorage.push_back(proc);

    // Dump the node tree, similar with the format in Lab task 4
    // root -> dump(0);
    // Dump the blocks, similar with the format in Lab task 4
    // for(auto i : Procedure::procStorage) i->dump();

    // Output dot graph
    ofstream SaveDot("cfg.dot");
    SaveDot << wrap_dot(Procedure::procStorage);
    SaveDot.close();

    // Output target code
    ofstream SaveTarget("target.cc");
    SaveTarget << wrap_cc(Procedure::procStorage);
    SaveTarget.close();

    return 0;
}

