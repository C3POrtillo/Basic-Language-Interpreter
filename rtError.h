/*
 * rtError.h
 */


#include "value.h"
using namespace std;

static bool error = false;

static void runTimeError(int line, Value err) {
	if (!error && err.isError()) {
		cerr << line << ": " << err << endl;
		error = true;
	}

}

