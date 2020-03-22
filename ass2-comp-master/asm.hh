#ifndef ASM_H
#define ASM_H

#include <iostream>
#include <string>
#include <set>
using namespace std;

string to_asm_ass(string, string);

void asm_add(string, string, string);
void asm_sub(string, string, string);
void asm_mul(string, string, string);
void asm_div(string, string, string);
string get_asm();

#endif