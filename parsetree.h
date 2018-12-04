/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include "value.h"
#include "rtError.h"

using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType {
	ERRTYPE, INTTYPE, STRTYPE, BOOLTYPE
};

class ParseTree {
	int linenum;

public:

	ParseTree *left;
	ParseTree *right;

	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0) :
			linenum(linenum), left(l), right(r) {

	}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLinenum() const {
		return linenum;
	}

	virtual NodeType GetType() const {
		return ERRTYPE;
	}

	int LeafCount() const {
		int lc = 0;
		if (left)
			lc += left->LeafCount();
		if (right)
			lc += right->LeafCount();
		if (left == 0 && right == 0)
			lc++;
		return lc;
	}

	virtual bool IsIdent() const {
		return false;
	}
	virtual bool IsString() const {
		return false;
	}

	virtual bool IsInt() const {
		return false;
	}
	virtual bool IsBool() const {
		return false;
	}

	virtual string GetId() const {
		return "";
	}

	virtual Value Eval() {
		static map<string, Value> symbolTable;
		return Eval(&symbolTable);
	}

	virtual Value Eval(map<string, Value> *symbolTable) const {
		Value err = Value("Invalid ParseTree", true);
		runTimeError(this->GetLinenum(), err);
		return err;
	}

};

class StmtList: public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) :
			ParseTree(0, l, r) {
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError((this->GetLinenum()), l);
			return l;
		}
		if (right != 0) {
			Value r = right->Eval(symbolTable);
			if (r.isError()) {
				runTimeError(this->GetLinenum(), r);
				return r;
			}
		}
		return l;
	}

};

class IfStatement: public ParseTree {
public:
	IfStatement(int line, ParseTree *ex, ParseTree *stmt) :
			ParseTree(line, ex, stmt) {
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError() || !l.isBoolType()) {
			Value err = Value("Invalid Boolean Expression inside if", true);
			runTimeError(this->GetLinenum(), err);
			return err;
		}
		if (l.getBoolean()) {
			Value r = right->Eval(symbolTable);
			if (r.isError()) {
				runTimeError(this->GetLinenum(), r);
				return r;
			}
		}
		return l;
	}

};

class Assignment: public ParseTree {
public:
	Assignment(int line, ParseTree *lhs, ParseTree *rhs) :
			ParseTree(line, lhs, rhs) {
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		if (left->IsIdent()) {
			Value r = right->Eval(symbolTable);

			if (r.isError()) {
				runTimeError(this->GetLinenum(), r);
				return r;
			}

			(*symbolTable)[left->GetId()] = r;
			return r;

		}
		Value err = Value("Invalid Assignment - Identifier cannot be resolved", true);
		runTimeError(this->GetLinenum(), err);
		return err;

	}
};

class PrintStatement: public ParseTree {
public:
	PrintStatement(int line, ParseTree *e) :
			ParseTree(line, e) {
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			Value err = Value("Invalid print", true);
			runTimeError(this->GetLinenum(), err);
			return err;
		}
		else {
			cout << l << endl;
		}
		return l;
	}

};

class PlusExpr: public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l + r;
	}

};

class MinusExpr: public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l - r;
	}

};

class TimesExpr: public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l * r;
	}

};

class DivideExpr: public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);

		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l / r;
	}

};

class LogicAndExpr: public ParseTree {
public:
	LogicAndExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l && r;
	}

};

class LogicOrExpr: public ParseTree {
public:
	LogicOrExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l || r;
	}

};

class EqExpr: public ParseTree {
public:
	EqExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l == r;
	}

};

class NEqExpr: public ParseTree {
public:
	NEqExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l != r;
	}

};

class LtExpr: public ParseTree {
public:
	LtExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l < r;
	}

};

class LEqExpr: public ParseTree {
public:
	LEqExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l <= r;
	}

};

class GtExpr: public ParseTree {
public:
	GtExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l > r;
	}

};

class GEqExpr: public ParseTree {
public:
	GEqExpr(int line, ParseTree *l, ParseTree *r) :
			ParseTree(line, l, r) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		Value l = left->Eval(symbolTable);
		if (l.isError()) {
			runTimeError(this->GetLinenum(), l);
			return l;
		}
		Value r = right->Eval(symbolTable);
		if (r.isError()) {
			runTimeError(this->GetLinenum(), r);
			return r;
		}
		return l >= r;
	}

};

class IConst: public ParseTree {
	int val;

public:
	IConst(int l, int i) :
			ParseTree(l), val(i) {
	}
	IConst(Token& t) :
			ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

	NodeType GetType() const {
		return INTTYPE;
	}

	bool IsInt() const {
		return true;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		//cout << "Iconst: " << val << endl;
		return Value(val);
	}

};

class BoolConst: public ParseTree {
	bool val;

public:
	BoolConst(Token& t, bool val) :
			ParseTree(t.GetLinenum()), val(val) {
	}

	NodeType GetType() const {
		return BOOLTYPE;
	}

	bool IsBool() const {
		return true;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		//cout << "Bool " << val << endl;
		return Value(val);
	}

};

class SConst: public ParseTree {
	string val;

public:
	SConst(Token& t) :
			ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}

	NodeType GetType() const {
		return STRTYPE;
	}
	bool IsString() const {
		return true;
	}
	Value Eval(map<string, Value> *symbolTable) const override {
		//cout << "Sconst: " << val << endl;
		return Value(val);
	}

};

class Ident: public ParseTree {
	string id;

public:
	Ident(Token& t) :
			ParseTree(t.GetLinenum()), id(t.GetLexeme()) {
	}

	bool IsIdent() const {
		return true;
	}
	string GetId() const {
		return id;
	}

	Value Eval(map<string, Value> *symbolTable) const override {
		if (symbolTable->find(id) == symbolTable->end()) {
			return Value("Identifier not found", true);
		}
		return (*symbolTable)[id];
	}

};

#endif /* PARSETREE_H_ */
