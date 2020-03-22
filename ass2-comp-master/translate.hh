#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include "asm.hh"
using namespace std;

class Global
{
public:
    static set<string> all_variables;
    static unordered_map<string, int> all_tables;
};

class ThreeAd
{
public:
    string name, lhs, rhs;
    char op;

    ThreeAd(string name, char op, string lhs, string rhs)
        : name(name), op(op), lhs(lhs), rhs(rhs) {}

    void dump()
    {
        cout << name << " <- ";
        cout << lhs << " " << op << " " << rhs << endl;
    }

    string to_string()
    {
        string n_rhs = rhs;
        if (n_rhs.substr(0,1) == "\"") {
            n_rhs = "\\" + n_rhs;
            n_rhs.insert(n_rhs.size() - 1, "\\");
        }
        return name + " := " + lhs + " " + op + " " + n_rhs;
    }

    string to_c()
    {
        switch(op)
        {
            // case '+' : return name + " = " + lhs + " + " + rhs + ";";
            // case '-' : return name + " = " + lhs + " - " + rhs + ";";
            // case '*' : return name + " = " + lhs + " * " + rhs + ";";
            // case '/' : return name + " = (float)" + lhs + " / " + rhs + ";";
            case '+' :
                asm_add(name, lhs, rhs);
                return "// add asm merged";
            case '-' :
                asm_sub(name, lhs, rhs);
                return "// sub asm merged";
            case '*' :
                asm_mul(name, lhs, rhs);
                return "// mul asm merged";
            case '/' :
                asm_div(name, lhs, rhs);
                return "// div asm merged";
            case '%' : return name + " = (int)" + lhs + " % (int)" + rhs + ";";
            case '#' : {
                auto search = Global::all_tables.find(lhs);
                return get_asm() + name + " = "+ std::to_string(search->second - 1) + ";";
            }
            case 'c' :
                if (lhs == "true") lhs = "1";
                if (lhs == "false") lhs = "0";
                return get_asm() + to_asm_ass(name, lhs);
            case '^' : return get_asm() + name + " = " + "pow(" + lhs + ", " + rhs + ");";
            case 'R' : return get_asm() + "return " + lhs + ";";
            case '>' : return get_asm() + "// Jump condition(Greater)";
            case '<' : return get_asm() + "// Jump condition(Less)";
            case '=' : return get_asm() + "// Jump condition(Equal)";
            case 'f' :
                if (lhs == "print") {
                    if (rhs.substr(0,1) == "\"") return get_asm() + "printf(\"%s\\n\", " + rhs + ");";
                    else return get_asm() + "printf(\"%.2f\\n\", " + rhs + ");";
                }
                else if (lhs == "io.read")
                    return get_asm() + "scanf(\"%lf\", &" + name + ");";
                else if (lhs == "io.write")
                    if (rhs.substr(0,1) == "\"") return get_asm() + "printf(\"%s\", " + rhs + ");";
                    else return get_asm() + "printf(\"%.2f\", " + rhs + ");";
                else return get_asm() + name + " = " + lhs + "(" + rhs + ");";
        }
        return "";
    }
};

class BBlock
{
public:
    static int nCounter;
    list<ThreeAd> instructions;
    BBlock *tExit, *fExit;
    string name;
    BBlock() : tExit(NULL), fExit(NULL), name("blk" + to_string(nCounter++)) {}
    
    void dump()
    {
        cout << "BBlock @" << this << endl;
        cout << name << endl;
        for(auto i : instructions) i.dump();
        cout << "True: " << tExit << endl;
        cout << "False: " << fExit << endl;
    }

    string to_dot()
    {
        string result;
        result = result + name + "[label=\"";
        for(auto i : instructions)
            result = result + i.to_string() + "\\n";
        result = result + "\"]\n";
        if(tExit != nullptr) result = result + name + "->" + tExit->name + "[label=\"true\"]\n";
        if(fExit != nullptr) result = result + name + "->" + fExit->name + "[label=\"false\"]\n";
        return result;
    }

    string to_c()
    {
        string result;
        // block label
        result = result + name + ":\n";
        // convert instructions to c code
        for(auto i : instructions)
            result = result + i.to_c() + "\n";
        // jump
        if(tExit != nullptr && fExit != nullptr)
        {
            ThreeAd cond = instructions.back();
            string cond_operator(1, cond.op);
            string cond_rhs = cond.rhs;
            if (cond_operator == "=") cond_operator = " == ";
            if (cond_rhs == "true") cond_rhs = "1";
            if (cond_rhs == "false") cond_rhs = "0";
            string cond_str = cond.lhs + cond_operator + cond_rhs;
            result = result + "if(" + cond_str + ") goto " + tExit->name + ";\n"
                            + "else goto " + fExit->name + ";\n";
        }
        if(tExit != nullptr && fExit == nullptr) result = result += "goto " + tExit->name + ";\n";
        if(tExit == nullptr && fExit == nullptr) result += "return 0;\n";
        return result += "\n";
    }
};

class Procedure
{
public:
    static list<Procedure*> procStorage;
    BBlock *proc_root_blk;
    string proc_name, proc_argument;

    Procedure(string n, string a, BBlock *s) : proc_name(n), proc_argument(a), proc_root_blk(s) {}

    string to_dot() {
        string result = "";
        set<BBlock *> done, todo;
        todo.insert(proc_root_blk);
        while(todo.size()>0) {
            auto first = todo.begin();
            BBlock *next = *first;
            todo.erase(first);
            result += next->to_dot();
            done.insert(next);
            if(next->tExit!=NULL && done.find(next->tExit)==done.end()) todo.insert(next->tExit);
            if(next->fExit!=NULL && done.find(next->fExit)==done.end()) todo.insert(next->fExit);
        }
        return result;
    }

    string to_c() {
        string result = "";
        if(proc_name == "main")
            result += "int main() {\n\n";
        else
            result = result + "double " + proc_name + "(double " + proc_argument + ") { \n";

        // result += "double ";
        for (auto iter = Global::all_variables.cbegin(); iter != Global::all_variables.cend(); iter++) {
            if((*iter) != proc_argument) result = result + "double " + (*iter) + ";\n";
            // set <string>::iterator iter2 = iter;
            // ++iter2;
            // if(iter2 != all_variables.cend()) result += ", ";
        }
        for (auto iter = Global::all_tables.cbegin(); iter != Global::all_tables.cend(); iter++) {
            auto search = Global::all_variables.find("*" + iter->first);
            if(search == Global::all_variables.end()) {
                result = result + "double " + iter->first + "[" + to_string(iter->second) + "];\n";
            }
        }
        result += "\n";

        set<BBlock *> done, todo;
        todo.insert(proc_root_blk);
        while(todo.size()>0) {
            auto first = todo.begin();
            BBlock *next = *first;
            todo.erase(first);
            result += next->to_c();
            done.insert(next);
            if(next->tExit!=NULL && done.find(next->tExit)==done.end()) todo.insert(next->tExit);
            if(next->fExit!=NULL && done.find(next->fExit)==done.end()) todo.insert(next->fExit);
        }
        result += "}\n";
        return result;
    }

    void dump() {
        set<BBlock *> done, todo;
        todo.insert(proc_root_blk);
        while(todo.size()>0) {
            auto first = todo.begin();
            BBlock *next = *first;
            todo.erase(first);
            next -> dump();
            done.insert(next);
            if(next->tExit!=NULL && done.find(next->tExit)==done.end()) todo.insert(next->tExit);
            if(next->fExit!=NULL && done.find(next->fExit)==done.end()) todo.insert(next->fExit);
        }
    }
};

#endif
