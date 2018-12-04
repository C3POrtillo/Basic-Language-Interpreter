/*
 * TokenReader.cpp
 *
 *  Created on: Oct 13, 2018
 *      Author: Camilo III P. Ortillo
 */

#include <map>
#include <string.h>
#include "tokens.h"

using namespace std;

const map<string, TokenType> tokenMap = { { "print", PRINT }, { "if", IF }, { "then", THEN }, { "true", TRUE }, {
		"-true", FALSE }, { "false", FALSE }, { "-false", TRUE }, { "+", PLUS }, { "-", MINUS }, { "*", STAR }, { "/",
		SLASH }, { "=", ASSIGN }, { "==", EQ }, { "!=", NEQ }, { "<", LT }, { "<=", LEQ }, { ">", GT }, { ">=", GEQ }, {
		"&&", LOGICAND }, { "||", LOGICOR }, { "(", LPAREN }, { ")", RPAREN }, { ";", SC } };

Token getNextToken(istream *in, int *linenum) {
	enum LexState {
		BEGIN, INID, INSTRING, INNUM, OPERATOR,
	};

	LexState lexstate = BEGIN;
	string lexeme = "";
	char ch;
	while (in->get(ch), *in) {

		if (ch == '\n') {
			(*linenum)++;
		}

		switch (lexstate) {
		case BEGIN:

			// Ignore whitespace
			if (isspace(ch)) {
				continue;
			}
			lexeme = "";
			if (isalpha(ch)) {
				lexstate = INID;
			}
			else if (isdigit(ch)) {
				lexstate = INNUM;
			}
			else if (ch == '"') {
				lexeme = ""; // reset lexeme for string, ignore beginning "
				lexstate = INSTRING;
			}

			else if (ch == '#') {
				while (ch != '\n') {
					ch = in->get();
				}
				(*linenum)++;
				break;
			}
			else if (ispunct(ch)) {
				lexstate = OPERATOR;
			}

			// no break
		case INID:
			if (isalpha(ch) || isdigit(ch)) {
				lexeme += ch;
			}
			else {
				if (ch == '\n') {
					(*linenum)--;
				}
				in->putback(ch);
				map<string, TokenType>::const_iterator it = tokenMap.find(lexeme);
				// Check for keywords
				if (it != tokenMap.end()) {
					return Token(it->second, lexeme, *linenum);
				}
				else {
					if (lexeme.size() != 0) {
						return Token(IDENT, lexeme, *linenum);
					}
				}
			}
			break;

		case INSTRING:
			if (ch == '"') {
				if (lexeme.size() != 0) {
					return Token(SCONST, lexeme, *linenum);
				}
			}
			else if (ch == '\n') {
				return Token(ERR, "\"" + lexeme + "\n", *linenum);
			}
			else {
				lexeme += ch;
			}
			break;

		case INNUM:
			if (isdigit(ch)) {
				lexeme += ch;
			}
			else if (isalpha(ch)) {
				return Token(ERR, lexeme + ch, *linenum);
			}
			else {
				if (ch == '\n') {
					(*linenum)--;
				}
				in->putback(ch);
				return Token(ICONST, lexeme, *linenum);
			}
			break;
		case OPERATOR:
			//+ - * / ( ) ; = == != > >= < <= && ||
			lexeme = ch;

			char temp = in->get();

			if (temp == '\n') {
				(*linenum)++;
			}
			map<string, TokenType>::const_iterator it = tokenMap.find(lexeme + temp);
			if (it != tokenMap.end()) {

				return Token(it->second, lexeme + temp, *linenum);
			}
			else if (tokenMap.find(lexeme) != tokenMap.end()) {
				it = tokenMap.find(lexeme);
				if (temp == '\n') {
					(*linenum)--;
				}
				in->putback(temp);

				return Token(it->second, lexeme, *linenum);
			}
			else {
				return Token(ERR, lexeme, *linenum);
			}
		}
	}
	return Token(DONE, lexeme, *linenum);
}
