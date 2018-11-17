#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <iostream>
#include "API.h"
#include "catalogmanager.h"

using namespace std;
class Interpreter {
public:

	string fileName;
	API api;

	Interpreter();
	string getCommand();
	string getword(string &SQL);
	string regular_SQL(string sql);
	int runSQL(string &sql);
	int  select(string &sql);
	int  drop(string &sql);
	int  create(string& sql);
	int tableCreate(string &sql);
	int indexCreate(string & sql);
	int indexDrop(string &sql);
	int tableDrop(string & sql);
	int insert(string &sql);
	int valueDrop(string &sql);
	int quit();
	int execfile(string &sql);
	
};

#endif	