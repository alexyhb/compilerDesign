#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <cmath>

class Node
{
public:
    static std::unordered_map<std::string, Node> storage;
    std::string tag, value;
    std::list<Node> children;
    Node(std::string t, std::string v) : tag(t), value(v) {}

    Node() {
        tag = "uninitialised";
        value = "uninitialised";
    } // Bison needs this.

    void dump(int depth = 0) {
        for(int i=0 ; i<depth ; i++)
            std::cout << "  ";
        std::cout << tag << ":" << value << std::endl;
        for (auto i = children.begin(); i != children.end(); i++)
            (*i).dump(depth + 1);
    }

    std::string dump_to_dot() { return "digraph {\n" + dump_to_dot(0, 0, "") + "}"; }

    Node eval() {

        if(tag == "chunk" || tag == "explist" || tag == "block" || tag == "varlist") {
            // last as result
            Node result;
            for (int i = 0; i < children.size(); i++) {
                Node current = child(i);
                result = current.eval();
                if(current.tag == "if_stat" && result.tag == "chunk") {
                    for(int t = i + 1; t < children.size(); t++) {
                        result.children.push_back(child(t));
                    }
                    return result.eval();
                }
                if(current.tag == "laststat") return result;
            }
            return result;
        } else

        if(tag == "assign_stat") {
            assign(child(0), child(1));
            return Node();
        } else

        if(tag == "fun_call_stat" || tag == "functioncall") {

            if(child(0).value == "print") return print();
            if(child(0).value == "io.read") return io_read();
            if(child(0).value == "io.write") return io_write();
            // self-design funciton
            std::string func_name = child(0).value;
            Node func = read_from_storage(func_name);
            Node exp = child(1).child(0).eval();
            // create_func_call(func, exp).dump();
            return create_func_call(func, exp).eval();
        } else

        if(tag == "laststat") {
            return child(0).eval();
        } else

        if(tag == "for_stat") {
            write_to_storage(child(0).value, child(1).eval());
            double i = getVarNum(child(0));
            double n;
            if(child(2).tag == "exp")
                n = std::stod(child(2).eval().value);
            else
                n =  getVarNum(child(2));
            do {
                child(3).eval();
                i++;
                write_to_storage(child(0).value, Node("num_exp", std::to_string(i)));
            } while(n + 1 - i > 0);
        } else

        if(tag == "if_stat") {
            // eval if_stat will return a block node
            std::string temp;
            if(child(0).eval().tag == "var") // first node is var
                temp = read_from_storage(child(0).eval().value).value;
            else // first node is exp
                temp = child(0).eval().value;
            if(temp == "true")
                return child(1).child(0);
            else
                if (children.size() == 3) return child(2).child(0);
                else return Node();
        } else

        if(tag == "repeat_stat") {
            do {
                child(0).eval();
            } while(child(1).eval().value != "true");
        } else

        if(tag == "func_stat") {
            std::string func_name = child(0).child(0).value;
            write_to_storage(func_name, child(1));
        } else

        if(tag == "exp") {
            if(value == "unop") {
                std::string op = child(0).value;
                if (op == "#") {
                    Node list = read_from_storage(child(1).value);
                    return Node("num_exp", std::to_string(list.children.size()));
                }
            }
            if(value == "false" || value == "true") return *this;
            if(value == "table") return child(0);
            if(value == "eq") {
                // left is var, right is bool exp
                if(child(1).eval().value == "false" || child(1).eval().value == "true") {
                    std::string temp = read_from_storage(child(0).value).value == child(1).eval().value ? "true" : "false";
                    return Node("exp", temp);
                }
                // num to num
                if(std::fabs(getVarNum(child(0).eval()) - getVarNum(child(1).eval())) < 0.001)
                    return Node("exp", "true");
                else
                    return Node("exp", "false");
            }
            double left = getVarNum(child(0).eval());
            double right = getVarNum(child(1).eval());
            double result;
            if(value == "add") result = left + right;
            if(value == "sub") result = left - right;
            if(value == "mul") result = left * right;
            if(value == "div") result = left / right;
            if(value == "pow") result = std::pow(left, right);
            if(value == "gt") {
                std::string is_bigger = left - right > 0.000001 ? "true" : "false";
                return Node("exp", is_bigger);
            }
            if(value == "lt") {
                std::string is_bigger = right - left > 0.000001 ? "true" : "false";
                return Node("exp", is_bigger);
            }
            if(value == "mod") result = std::fmod(left, right);
            return Node("num_exp", std::to_string(result));
        } else

        if(tag == "var" || tag == "STRING" || tag == "num_exp") {
            return *this;
        } else

        if(tag == "barket_var") {
            Node list = read_from_storage(child(0).value);
            int index;
            if(child(1).tag == "var")
                index = getVarNum(child(1));
            else
                index = std::stoi(child(1).eval().value);
            return list.child(index - 1).eval();
        }

        return Node();
    }

    Node child(int i) {
        std::vector<Node> temp{ std::begin(children), std::end(children) };
        return temp[i];
    }

    double getVarNum(Node n) {
        std::string result;
        if(n.tag == "num_exp")
            result = n.value;
        else if(n.tag == "var") {
            // if(search == storage.end()) TBD: need to handle: if variable is not exist
            auto search = storage.find(n.value);
            Node found = search->second;
            result = found.value;
        }
        return std::stod(result);
    }

private:
    std::string dump_to_dot(int depth, int count, const std::string &father_name) {
        std::stringstream ss;
        std::stringstream temp;
        temp << father_name << "x" << count;
        std::string node_name = temp.str();
        ss << node_name << " [label=\"" << tag << "\\n" << value << "\"]" << std::endl;
        int child_count = 0;
        for (auto &i : children) {
            ss << node_name << " -> " << node_name << "x" << child_count << std::endl;
            ss << i.dump_to_dot(depth + 1, child_count++, node_name);
        }
        return ss.str();
    }

    void write_to_storage(std::string key, Node value) {
        auto it = storage.find(key);
        if (it == storage.end())
            storage.insert({key, value});
        else // update
            it->second = value;
    }

    void write_to_table(std::string table_name, int index, Node value) {
        Node table = read_from_storage(table_name);
        Node new_table = Node("fieldlist", "");
        for(int i = 0; i < table.children.size(); i++) {
            if(i == index) {
                new_table.children.push_back(value);
            }
            else
                new_table.children.push_back(table.child(i));
        }
        write_to_storage(table_name, new_table);
    }

    void assign(Node varlist, Node explist) {
        int size = varlist.children.size();
        if (size == 1)
            write_to_storage(varlist.eval().value, explist.eval());
        else {
            Node explist_result = Node();
            for (auto &i : explist.children)
                explist_result.children.push_back(i.eval());
            for (int i = 0; i < size; i++) {
                Node left = varlist.child(i);
                Node right = explist_result.child(i);
                if (left.tag == "barket_var") {
                    int index;
                    if (left.child(1).eval().tag == "var")
                        index = getVarNum(left.child(1).eval());
                    else
                        index = std::stoi(left.child(1).eval().value);
                    write_to_table(left.child(0).value, index - 1, right);
                }
            }
        }
    }

    Node read_from_storage(std::string key) {
        return storage.find(key)->second;
    }

    Node create_func_call(Node funcbody, Node exp) {
        // replace function paramater to expression
        // only support one now
        std::string para_name = funcbody.child(0).child(0).child(0).value;
        Node func_block = funcbody.child(1).child(0);
        return replace(func_block, para_name, exp);
    }

    Node replace(Node target, std::string name, Node exp) {
        if (target.children.size() > 0) {
            std::list<Node> temp;
            for (auto &i : target.children)
                temp.push_back(replace(i, name, exp));
            target.children = temp;
            return target;
        } else if (target.value == name)
            return exp;
        else
            return target;
    }

    Node io_read() {
        std::string value;
        getline(std::cin, value, '\n');
        try {
            double val = std::stoi(value);
            return Node("num_exp", value);
        } catch(std::invalid_argument) {
            std::cout << "invalid input" << std::endl;
            exit(-1);
        }
    }
    Node io_write() {
        if (child(1).tag == "STRING")
            std::cout << child(1).value;
        else {
            for (auto &i : child(1).children)
                if(i.tag == "STRING")  std::cout << i.value;
                else std::cout << getVarNum(i.eval());
        }
        return Node();
    }
    Node print() {
        if (child(1).tag == "STRING")
            std::cout << child(1).value << std::endl;
        else {
            for (auto &i : child(1).children)
                std::cout << getVarNum(i.eval()) << "\t";
            std::cout << std::endl;
        }
        return Node();
    }
};