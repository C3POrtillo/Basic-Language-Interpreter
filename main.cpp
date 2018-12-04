/*
 * main.cpp
 *
 *  Created on: Nov 6, 2018
 *      Author: Camilo III P. Ortillo
 */

#include "tokens.h"
#include "parse.h"
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
	ifstream file;
	istream *in;
	int linenum = 0;

	if (argc == 1) {
		in = &cin;
	}

	else if (argc == 2) {
		file.open(argv[1]);
		if (file.is_open() == false) {
			cerr << "COULD NOT OPEN " << argv[1] << endl;
			return 1;
		}
		in = &file;
	}

	else {
		cerr << "TOO MANY FILENAMES" << endl;
		return 1;
	}

	ParseTree *prog = Prog(in, &linenum);

	if (prog == 0) {
		return 0;
	}
	prog->Eval();

}
