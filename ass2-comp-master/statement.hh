#include <list>
#include "expression.hh"
using namespace std;

class Stmt
{
public:
    virtual void dump(int indent) = 0;
    virtual BBlock* convert(BBlock *out) {
       return out;
    }
};

class StmtList : public Stmt
{
public:
    list<Stmt*> children;
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Stmtlist>" << endl;
        indent++;
        for(auto i : children) i -> dump(indent);
    }
    BBlock* convert(BBlock *out)
    {
        BBlock *cur = out;
        for(auto i : children)
        {
            cur = i -> convert(cur);
            while(cur -> tExit != nullptr) cur = cur -> tExit;
        }
        return out;
    }
};

class Assignment : public Stmt
{
public:
    ExpList *lhs, *rhs;
    Assignment(ExpList *lhs, ExpList *rhs)
        : lhs(lhs), rhs(rhs) {}

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Assignment>" << endl;
        lhs -> dump(++indent);
        rhs -> dump(indent);
    }

    BBlock *convert(BBlock *out)
    {
        if (lhs -> children.size() == 2)
        {
            Exp *temp = new ExpList();
            Exp *x = lhs -> get(0);
            Exp *y = lhs -> get(1);
            string tname = temp->makeNames();
            string xname = x -> convert(out);
            string yname = y -> convert(out);
            out -> instructions.push_back(ThreeAd(tname, 'c', xname, xname));
            out -> instructions.push_back(ThreeAd(xname, 'c', yname, yname));
            out -> instructions.push_back(ThreeAd(yname, 'c', tname, tname));

        } else {
            Exp *sublhs = lhs -> get(0);
            Exp *subrhs = rhs -> get(0);
            string rname = subrhs -> convert(out);
            string lname = sublhs -> convert(out);
            auto search = Global::all_tables.find(rname);
            if(search != Global::all_tables.end()) {
                Global::all_variables.erase(Global::all_variables.find(lname));
                Global::all_variables.insert("*"+lname);
                Global::all_tables.insert({lname, search->second});
            }
            out -> instructions.push_back(ThreeAd(sublhs->name, 'c', rname, rname));
        }
        return out;
    }
};

class For : public Stmt
{
public:
    string var;
    Exp *init, *end;
    StmtList *block;
    For(string v, Exp *i, Exp *e, StmtList *b)
        : var(v), init(i), end(e), block(b)
    {
    }
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<For>" << var << endl;
        init -> dump(++indent);
        end -> dump(indent);
        block -> dump(indent);
    }
    BBlock *convert(BBlock *out)
    {
        init -> convert(out);
        out -> instructions.push_back(ThreeAd(var, 'c', init->name, init->name));
        end -> convert(out);
        out -> instructions.push_back(ThreeAd(out->name + "_cond", '>', var, end->name));
        BBlock* loopBody = new BBlock();
        if(out -> tExit == nullptr) out -> tExit = new BBlock();
        out->fExit = block->convert(loopBody);
        Exp *temp = new ExpList();

        BBlock* cur = loopBody;
        while(cur -> tExit != nullptr) cur = cur -> tExit;
        cur->instructions.push_back(ThreeAd(temp->makeNames(), '+', var, "1"));
        cur->instructions.push_back(ThreeAd(var, 'c', temp->makeNames(), temp->makeNames()));
        cur->instructions.push_back(ThreeAd(cur->name + "_cond", '>', var, end->name));
        cur->tExit = out->tExit;
        cur->fExit = loopBody;
        return out;
    }
};

class IfThenElse : public Stmt
{
public:
    Exp *cond;
    StmtList *then_stmt, *else_stmt;
    IfThenElse(Exp *c, StmtList *t, StmtList *e)
        : cond(c), then_stmt(t), else_stmt(e) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<IfThenElse>" << endl;
        cond -> dump(++indent);
        then_stmt -> dump(indent);
        else_stmt -> dump(indent);
    }
    BBlock *convert(BBlock *out)
    {
        cond -> convert(out);
        
        out -> tExit = new BBlock();

        BBlock *thenBlock = then_stmt->convert(new BBlock());
        BBlock *elseBlock = else_stmt->convert(new BBlock());

        BBlock *cur = thenBlock;
        while(cur -> tExit != nullptr) cur = cur -> tExit;
        cur->tExit = out -> tExit;

        cur = elseBlock;
        while(cur -> tExit != nullptr) cur = cur -> tExit;
        cur->tExit = out -> tExit;

        out->tExit = thenBlock;
        out->fExit = elseBlock;

        return out;
    }
};

class IfThen : public Stmt
{
public:
    Exp* cond;
    StmtList *then_stmt;
    IfThen(Exp *c, StmtList *t)
        : cond(c), then_stmt(t) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<IfThen>" << endl;
        cond -> dump(++indent);
        then_stmt -> dump(indent);
    }
    BBlock *convert(BBlock *out)
    {
        cond -> convert(out);
        
        out -> tExit = new BBlock();

        BBlock *thenBlock = then_stmt->convert(new BBlock());

        BBlock *cur = thenBlock;
        while(cur -> tExit != nullptr) cur = cur -> tExit;
        cur->tExit = out -> tExit;

        out->fExit = out->tExit;
        out->tExit = thenBlock;
        

        return out;
    }
};

class Repeat : public Stmt
{
public:
    StmtList *block;
    Exp *cond;
    Repeat(StmtList *b, Exp *c) : block(b), cond(c) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<Repeat>" << endl;
        block -> dump(++indent);
        cond -> dump(indent);
    }

    BBlock *convert(BBlock *out)
    {
        BBlock* loopBody = new BBlock();
        BBlock* afterBlock = out->tExit;
        if (afterBlock == nullptr) afterBlock = new BBlock();
        block->convert(loopBody);
        out->tExit = loopBody;

        BBlock* cur = loopBody;
        while(cur -> tExit != nullptr) cur = cur -> tExit;

        cond -> convert(cur);

        cur->tExit = afterBlock;
        cur->fExit = loopBody;
        return out;
    }
};

class LastStmt : public Stmt
{
public:
    Exp *ex;
    LastStmt(Exp *e) : ex(e) {}

    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<LastStmt>" << endl;
        ex -> dump(++indent);
    }

    BBlock *convert(BBlock *out)
    {
        ex->convert(out);
        out -> instructions.push_back(ThreeAd(out->name+"_ret", 'R', ex->name, ex->name));
        return out;
    }
};

class FuncDef : public Stmt
{
public:
    string fname, aname;
    StmtList *block;
    FuncDef(string f, string a, StmtList *b)
        : fname(f), aname(a), block(b) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<FuncDef>" << fname << "(" << aname << ")" << endl;
        block -> dump(++indent);
    }
    BBlock* convert(BBlock *out)
    {
        Procedure *proc = new Procedure(fname, aname, block -> convert(new BBlock()));
        Procedure::procStorage.push_back(proc);
        return out;
    }
};

class FunCallStmt : public Stmt
{
public:
    Exp *funcall;
    FunCallStmt(Exp *f) : funcall(f) {}
    void dump(int indent)
    {
        for(int i = 0; i < indent; i++) cout << "  ";
        cout << "<FunCallStmt>" << endl;
        funcall -> dump(++indent);
    }
    BBlock* convert(BBlock *out)
    {
        funcall -> convert(out);
        return out;
    }
};