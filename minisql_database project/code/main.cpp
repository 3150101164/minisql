#include "API.h"
#include "BufferManager.h"
#include "catalogmanager.h"
#include "IndexManager.h"
#include "Interpreter.h"
#include "MiniSQL.h"
#include "recordmanager.h"
//#include "BufferManager.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <fstream>

int main() {
	//API api;
	//record_management rm;
	catalogmanager cm;
	//api.catalog = &cm;
	//api.record1 = &rm;
	Interpreter myinterpreter;
	string filename;
	while (cin) {
		cout << "miniSQL>>";
		string s = myinterpreter.getCommand();
		s = myinterpreter.regular_SQL(s);
		int i = myinterpreter.runSQL(s);
	}
}