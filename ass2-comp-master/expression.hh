#include <vector>
#include "translate.hh"
using namespace std;

/************* Expressions **************/
class Exp
{
public:
    string name;
    static int nameCounter;
    virtual void dump(int indent) = 0;

    Exp() : name("") {}

    virtual string makeNames()
    {
        if(name == "") name = "_t" + to_string(nameCounter++);
        Global::all_variables.insert(name);
        return name;
    }

    virtual string makeNames(string input)
    {
        name = input;
        Global::all_variables.insert(name);
        return name;
    }

    virtual string convert(BBlock* out) = 0;
};

class ExpList : public Exp
{
public:
    vector<Exp*> children;
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Explist>" << endl;
        indent++;
        for(auto i : children) i -> dump(indent);
    }
    string convert(BBlock *out)
    {
        string listname = makeNames();
        Global::all_variables.erase(Global::all_variables.find(listname));
        Global::all_tables.insert({listname, children.size() + 1});
        int index = 1;
        for(auto i : children)
        {
            i -> convert(out);
            out -> instructions.push_back(
                ThreeAd(listname + "[" + to_string(index++) + "]", 'c', i -> name, i -> name)
            );
        }
        return name;
    }
    Exp *get(int index)
    {
        return children[index];
    }
};

class Great : public Exp
{
public:
    Exp *lhs, *rhs;

    Great(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Great>==" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '>', lhs->convert(out), rhs->convert(out))
        );
        return name;
    }
};

class Less : public Exp
{
public:
    Exp *lhs, *rhs;

    Less(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Less>==" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '<', lhs->convert(out), rhs->convert(out))
        );
        return name;
    }
};

class Equality : public Exp
{
public:
    Exp *lhs, *rhs;

    Equality(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Equality>==" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '=', lhs->convert(out), rhs->convert(out))
        );
        return name;
    }
};

class Add : public Exp
{
public:
    Exp *lhs, *rhs;

    Add(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "+" << endl;
        indent += 1;
        lhs -> dump(indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '+', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Mul : public Exp
{
public:
    Exp *lhs, *rhs;
    
    Mul(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "*" << endl;
        indent += 1;
        lhs -> dump(indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '*', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Mod : public Exp
{
public:
    Exp *lhs, *rhs;
    
    Mod(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Mod>%" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }
    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '%', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Sub : public Exp
{
public:
    Exp *lhs, *rhs;
    
    Sub(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "-" << endl;
        indent += 1;
        lhs -> dump(indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '-', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Div : public Exp
{
public:
    Exp *lhs, *rhs;
    
    Div(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "/" << endl;
        indent += 1;
        lhs -> dump(indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '/', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Pow : public Exp
{
public:
    Exp *lhs, *rhs;
    
    Pow(Exp *lhs, Exp *rhs)
        : lhs(lhs), rhs(rhs)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "^" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '^', lhs->convert(out), rhs->convert(out))
        );
        
        return name;
    }
};

class Len : public Exp
{
public:
    Exp *ex;
    Len(Exp *e) : ex(e) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Len>#" << endl;
        ex -> dump(++indent);
    }

    string convert(BBlock *out)
    {
        out -> instructions.push_back(
            ThreeAd(makeNames(), '#', ex->convert(out), ex->convert(out))
        );
        
        return name;
    }
};

class Num : public Exp
{
public:
    string value;

    Num(string value)
        : value(value)
    {
        name = value;
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Num>" << value << endl;
    }

    string convert(BBlock *out) {
        return name;
    }
};

class String : public Exp
{
public:
    string value;

    String(string value)
        : value(value)
    {
        name = value;
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<String>" << value << endl;
    }

    string convert(BBlock *out) { 
        // string sb = value.substr(1, value.length()-2);
        // out -> instructions.push_back(
        //     ThreeAd(makeNames(), 'S', sb, sb)
        // );
        return name;
    }
};

class Var : public Exp
{
public:
    string value;
    Var(string value) : value(value)
    {
        name = value;
        Global::all_variables.insert(name);
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Var>" << value << endl;
    }

    string convert(BBlock *out) {
        
        return name;
    }
};

class Boolea : public Exp
{
public:
    string value;

    Boolea(string n) : value(n) {
        name = value;
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Boolean>" << value << endl;
    }

    string convert(BBlock *out) { return name; }
};

class SuffixVar : public Exp
{
public:
    string value;
    Exp *suf;
    SuffixVar(string n, Exp *f) : value(n), suf(f)
    {
        string expName = suf -> makeNames();
        name = value + "[(int)" + expName + "]";
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<SuffixVar>" << value << endl;
        suf -> dump(++indent);
    }

    string convert(BBlock *out)
    {
        suf->convert(out);
        return name;
    }
};

class Table : public Exp
{
public:
    ExpList *elist;
    Table() : elist(NULL) {}
    Table(ExpList *e) : elist(e) {}

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Table>" << endl;
        if (elist != nullptr)
            elist -> dump(++indent);
    }

    string convert(BBlock *out)
    {
        return elist -> convert(out);
    }
};

class FunCall : public Exp
{
public:
    string fname;
    ExpList* args;
    String* sarg;
    FunCall(string fname, ExpList *args)
        : fname(fname), args(args), sarg(NULL)
    {
    }

    FunCall(string fname, String* sarg)
        : fname(fname), args(NULL), sarg(sarg)
    {
    }

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<funcall>" << fname << endl;
        indent += 1;
        if(args != nullptr)
            args -> dump(indent);
        else
            sarg -> dump(indent);
    }

    string convert(BBlock *out)
    {
        if(args != nullptr)
            for (auto i : args->children)
                out -> instructions.push_back(ThreeAd(makeNames(), 'f', fname, i->convert(out)));
        else
            out -> instructions.push_back(ThreeAd(makeNames(), 'f', fname, sarg->convert(out)));
        return name;
    }
};