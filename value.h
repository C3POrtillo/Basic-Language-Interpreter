/*
 * value.h
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <string>
#include <iostream>
using namespace std;

// object holds boolean, integer, or string, and remembers which it holds
class Value {
	bool bval;
	int ival;
	string sval;
	enum VT {
		isBool, isInt, isString, isTypeError
	} type;

public:

	Value() :
			bval(false), ival(0), type(isTypeError) {
	}
	Value(bool bval) :
			bval(bval), ival(0), type(isBool) {
	}
	Value(int ival) :
			bval(false), ival(ival), type(isInt) {
	}
	Value(string sval) :
			bval(false), ival(0), sval(sval), type(isString) {
	}

	// in the case of an error, I use the value to hold the error message
	Value(string sval, bool isError) :
			bval(false), ival(0), sval(sval), type(isTypeError) {
	}

	bool isBoolType() const {
		return type == VT::isBool;
	}
	bool isIntType() const {
		return type == VT::isInt;
	}
	bool isStringType() const {
		return type == VT::isString;
	}
	bool isError() const {
		return type == VT::isTypeError;
	}
	bool hasMessage() const {
		return isError() && sval.size() > 0;
	}

	bool isTrue() const {
		return isBoolType() && bval;
	}
	bool getBoolean() const {
		if (!isBoolType())
			throw "Not boolean valued";
		return bval;
	}

	int getInteger() const {
		if (!isIntType())
			throw "Not integer valued";
		return ival;
	}

	string getString() const {
		if (!isStringType())
			throw "Not string valued";
		return sval;
	}

	string getMessage() const {
		if (!hasMessage())
			throw "No message";
		return sval;
	}

	friend ostream& operator<<(ostream& out, const Value& v) {
		if (v.type == VT::isBool)
			out << (v.bval ? "True" : "False");
		else if (v.type == VT::isInt)
			out << v.ival;
		else if (v.type == VT::isString)
			out << v.sval;
		else if (v.sval.size() > 0)
			out << "RUNTIME ERROR " << v.sval;
		else
			out << "TYPE ERROR" << v.sval;
		return out;
	}

	Value operator+(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival + v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval + v.sval);
		}
		Value ret = Value("Invalid operands for +", true);

		return ret;
	}

	Value operator-(const Value& v) {
		return (this->areInts(v)) ? Value(this->ival - v.ival) : Value("Invalid operands for -", true);
	}

	Value operator*(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival * v.ival);
		}
		if (this->isIntType() && v.isStringType()) {
			if (this->ival < 0) {
				return Value("Can't multiply string by a negative", true);
			}
			else {
				string val = "";
				for (int i = 0; i < this->ival; i++) {
					val += v.sval;
				}
				return Value(val);
			}
		}

		if (this->isStringType() && v.isIntType()) {
			if (v.ival < 0) {
				return Value("Can't multiply string by a negative", true);
			}
			else {
				string val = "";
				for (int i = 0; i < v.ival; i++) {
					val += this->sval;
				}
				return Value(val);
			}
		}
		if (this->isIntType() && v.isBoolType()) {
			return Value(!v.getBoolean());
		}
		return Value("Invalid operands for *", true);
	}
	Value operator/(const Value& v) {
		if (v.ival == 0) {
			Value ret = Value("Division by 0", true);
			return ret;
		}
		if (this->areInts(v)) {
			return Value(this->ival / v.ival);
		}
		Value ret = Value("Invalid operands for /", true);

		return ret;
	}

	Value operator<(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival < v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval.compare(v.sval) < 0);
		}
		Value ret = Value("Invalid operands for <", true);

		return ret;
	}
	Value operator<=(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival <= v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval.compare(v.sval) <= 0);
		}
		Value ret = Value("Invalid operands for <=", true);

		return ret;

	}
	Value operator>(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival > v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval.compare(v.sval) > 0);
		}
		Value ret = Value("Invalid operands for >", true);

		return ret;
	}
	Value operator>=(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival >= v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval.compare(v.sval) >= 0);
		}
		Value ret = Value("Invalid operands for >=", true);

		return ret;
	}
	Value operator==(const Value& v) {
		if (this->areInts(v)) {
			return Value(this->ival == v.ival);
		}
		if (this->areStrings(v)) {
			return Value(this->sval.compare(v.sval) == 0);
		}
		if (this->areBools(v)) {
			return Value(this->bval == v.bval);
		}
		Value ret = Value("Invalid operands for ==", true);

		return ret;
	}
	Value operator!=(const Value& v) {
		Value ans = this->operator==(v);
		if (ans.type != VT::isTypeError) {
			return Value(!ans.bval);
		}
		Value ret = Value("Invalid operands for !=", true);

		return ret;
	}

	Value operator&&(const Value& v) {
		if (this->isBoolType()) {
			if (!this->getBoolean()) {
				return Value(false);
			}
		}
		if (this->areBools(v)) {
			return Value(this->bval && v.bval);
		}
		Value ret = Value("Invalid operands for &&", true);

		return ret;
	}

	Value operator||(const Value& v) {
		if (this->isBoolType()) {
			if (this->getBoolean()) {
				return Value(true);
			}
		}
		if (this->areBools(v)) {
			return Value(this->bval || v.bval);
		}
		Value ret = Value("Invalid operands for ||", true);

		return ret;
	}

private:
	bool areInts(const Value& v) {
		return this->isIntType() && v.isIntType();
	}

	bool areStrings(const Value& v) {
		return this->isStringType() && v.isStringType();
	}

	bool areBools(const Value& v) {
		return this->isBoolType() && v.isBoolType();
	}
};

#endif /* VALUE_H_ */
