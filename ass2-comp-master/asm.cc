#include "asm.hh"

string assembler_template = "";
set<string> output_operands = {};
set<string> input_operands = {};
set<string> clobbers = {};

// if input string is a number
bool is_number(string in) {
    try { stod(in); }
    catch(invalid_argument ia) { return false; }
    return true;
}

// if number: 7 -> n7
string num_to_var(string in) {
    if(is_number(in)) return "n" + in;
    else return in;
}

// if number: 7 -> (double)7
string num_to_double(string in) {
    if(is_number(in)) return "(double)" + in;
    else return in;
}

string fix_suffix_var(string in) {
    try {
        int index = in.find("[");
        in.replace(index, 1, "_");
        index = in.find("]");
        in.replace(index, 1, "_");
        index = in.find("(");
        in.replace(index, 1, "_");
        index = in.find(")");
        in.replace(index, 1, "_");
    } catch (out_of_range e) {}
    return in;
}

void asm_add(string name, string lhs, string rhs) {
    string  res =  "";
            res += "\"movsd %[" + num_to_var(rhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"addsd %[" + num_to_var(lhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"movsd %%xmm0, %[" + name + "]\\n\\t\"\n";
    assembler_template += res;
    output_operands.insert(name);
    input_operands.insert(lhs);
    input_operands.insert(rhs);
    clobbers.insert("xmm0");
    clobbers.insert("cc");
}

void asm_sub(string name, string lhs, string rhs) {
    string  res =  "";
            res += "\"movsd %[" + num_to_var(lhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"subsd %[" + num_to_var(rhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"movsd %%xmm0, %[" + name + "]\\n\\t\"\n";
    assembler_template += res;
    output_operands.insert(name);
    input_operands.insert(lhs);
    input_operands.insert(rhs);
    clobbers.insert("xmm0");
    clobbers.insert("cc");
}

void asm_mul(string name, string lhs, string rhs) {
    string  res =  "";
            res += "\"movsd %[" + num_to_var(rhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"mulsd %[" + num_to_var(lhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"movsd %%xmm0, %[" + name + "]\\n\\t\"\n";
    assembler_template += res;
    output_operands.insert(name);
    input_operands.insert(lhs);
    input_operands.insert(rhs);
    clobbers.insert("xmm0");
    clobbers.insert("cc");
}

void asm_div(string name, string lhs, string rhs) {
    string  res =  "";
            res += "\"movsd %[" + num_to_var(lhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"divsd %[" + num_to_var(rhs) + "], %%xmm0\\n\\t\"\n";
            res += "\"movsd %%xmm0, %[" + name + "]\\n\\t\"\n";
    assembler_template += res;
    output_operands.insert(name);
    input_operands.insert(lhs);
    input_operands.insert(rhs);
    clobbers.insert("xmm0");
    clobbers.insert("cc");
}

string to_asm_ass(string name, string lhs) {
    string res = "asm(\n";
            // res += "\"movsd %[" + fix_suffix_var(num_to_var(lhs)) + "], %%xmm0\\n\\t\"\n";
            res += "\"movsd %[" + fix_suffix_var(num_to_var(lhs)) + "], %[" + fix_suffix_var(name) + "]\\n\\t\"\n";
            res += ": [" + fix_suffix_var(name) + "] \"=x\" (" + name + ")\n";
            res += ": [" + fix_suffix_var(num_to_var(lhs)) + "] \"x\" (" + num_to_double(lhs) + ")\n";
            res += ": \"cc\", \"xmm0\"\n";
    res += ");\n";
    return res;
}

string get_asm() {
    if (assembler_template == "") return "";
    string res = "asm(\n";
    res += assembler_template;
    res += ": ";
    int count = 0;
    for(auto i : output_operands) {
        res += "[" + fix_suffix_var(i) + "] \"=x\" (" + i + ")";
        if((++count)!=output_operands.size()) res += ",";
        res += "\n";
    }
    res += ": ";
    count = 0;
    for(auto i : input_operands) {
        res += "[" + fix_suffix_var(num_to_var(i)) + "] \"x\" (" + num_to_double(i) + ")";
        if((++count)!=input_operands.size()) res += ",";
        res += "\n";
    }
    res += ": ";
    count = 0;
    for(auto i : clobbers) {
        res += "\""+ i +"\"";
        if((++count)!=clobbers.size()) res += ", ";
        else res += "\n";
    }
    res += ");\n";
    // clear storage
    assembler_template = "";
    output_operands.clear();
    input_operands.clear();
    clobbers.clear();
    return res;
}
