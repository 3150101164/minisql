#include "interpreter.h"
#include<iostream>
//#include<stdio.h>
#include<vector>
#include<string>
#include<cstring>
#include "minisql.h"
#include<fstream>
using namespace std;


string Interpreter::getCommand() {
	string s;
	string sql;
	cin >> s;
	sql += s;
	sql += " ";
	while (s.find(';') == s.npos) {
		cin >> s;
		sql += s;
		sql += " ";
	}
	return sql;
}
string Interpreter::regular_SQL(string sql) {
	string result;
	string char_set = " , ; <> ( ) * <= >= #";
	for (int i = 0; i < sql.length(); i++) {
		if (sql[i] == '<') {
			if (sql[i + 1] == '>') {
				result += " <> ";
				i++;
			}
			else if (sql[i + 1] == '=') {
				result += " <= ";
				i++;
			}
			else {
				result = result + ' ' + sql[i] + ' ';
			}
		}
		else if (sql[i] == '>') {
			if (sql[i + 1] == '=') {
				result += " >= ";
				i++;
			}
			else {
				result += " > ";
			}
		}
		else if (sql[i] == '#') {
			result = result + sql[i] + ' ';
		}
		else if (char_set.find(sql[i]) != char_set.npos) {
			result = result + ' ' + sql[i] + ' ';
		}
		else {
			result += sql[i];
		}
	}
	return result;

}


Interpreter::Interpreter() {
	fileName = "111";
}
string Interpreter::getword(string &SQL)
{
	string word;
	bool flag = true;
	for (int i = 0; i<SQL.length(); i++)
	{
		if (SQL[i] != ' ')
		{
			word = word + SQL[i];
			if (SQL[i] == '\'')
			{
				flag = !flag;
			}
		}
		else
		{
			while (SQL[i] == ' '&&i + 1<SQL.length())
			{
				if (!flag)
					word = word + SQL[i];
				i++;
			}
			if (!flag) 
			{
				--i;
				continue;
			}
			else
			{
				SQL.erase(0, i);
				break;
			}
			
		}
	}
	return word;
}
int Interpreter::runSQL(string &sql) {
	string op, s;
	op = getword(sql);

	if (op == "select") {
		select(sql);
		return 1;
	}
	else if (op == "drop") {
		drop(sql);
		return 1;
	}
	else if (op == "create") {
		create(sql);
		return 1;
	}
	else if (op == "insert") {
		insert(sql);
		return 1;
	}
	else if (op == "quit") {
		quit();
		return 1;
	}
	else if (op == "execfile") {
		execfile(sql);
		return 1;
	}
	else if (op == "delete") {
		valueDrop(sql);
		return 1;
	}
	else if (op == "#") {
		return 1;
	}

	else {
		cout << "syntax error, please input again." << endl;
		return 1;
	}
}

int  Interpreter::create(string &sql) {
	string des;
	bool flag = true;
	des = getword(sql);
	if (des == "table") {
		tableCreate(sql);
		return 1;
	}
	else if (des == "index") {
		indexCreate(sql);
		return 1;
	}
	else {
		cout << "Expected table or index after create";
		return 0;
	}

}
int Interpreter::tableCreate(string &sql) {
	string tableName = "";
	int flag = 0;
	int count = 0;

	string s, s1, s2, s3, s4;
	vector<Attribute> attributes;
	tableName = getword(sql);
	if (!tableName.empty()) {
		s = getword(sql);
		if (!(s == "(")) {
			cout << "Expected '(' afer tableName";
			return 0;
		}
		else flag++;
		while (flag != 0 || !(s == ")")) {
			count = 0;
			vector<string> s_vec;
			s_vec.clear();
			while (1) {
				s = getword(sql);
				//if (s == ";")
				//	break;
				if (s == "(") {
					flag++;
					continue;
				}
				else if (s == ",")
					break;
				if (s == ")") {
					flag--;
					if (!flag)
						break;
					else
						continue;
				}
				count++;
				s_vec.push_back(s);
			}

			if (s_vec[0] == "primary") {
				for (int i = 2; i < count; i++) {
					for (int j = 0; j < attributes.size(); j++) {
						if (attributes[j].name == s_vec[i]) {
							attributes[j].isPrimary = true;
							attributes[j].isUnique = true;
							attributes[j].index = attributes[j].name;
						}
					}
				}
			}
			else {
				string name = s_vec[0];
				string index = "empty";
				int type;
				bool isUnique = false;
				bool isPrimary = false;
				string ty = s_vec[1];
				int TP = 0 ;
				int length = 0;
				if (ty == "float") {
					type = -1;
					TP = 3;
					length = 0;
				}
				else if (ty == "int")
				{
					type = 0;
					TP = 1;
					length = 0;
				}
				else if (ty == "char")
				{
					type = stoi(s_vec[2]);
					TP = 2; length = type;
				}
				if (count >= 3 && TP != 2) {
					if (s_vec[2] == "unique")
						isUnique = true;
				}
				else if (TP == 2 && count >= 4) {
					if (s_vec[3] == "unique")
						isUnique = true;
				}

				Attribute * att = new Attribute(name, isUnique, isPrimary, index, TP, length);
				attributes.push_back(*att);


				//cout << att->name << endl;
			}
		}
		//else
		//return 0;
		Table *tab = new Table(tableName, attributes, 0);
		api.createTable(tab, tableName, attributes);

	}
}
//至此已经计算出Attributes和tableName；
//}

//}

int Interpreter::indexCreate(string &sql) {
	string indexName;
	string tableName;
	string s;

	indexName = getword(sql);

	s = getword(sql);

	if (s != "on") {
		cout << "Expected \"on\" after " << indexName << endl;
		return 0;
	}

	tableName = getword(sql);

	s = getword(sql);
	if (s != "(") {
		cout << "Expected \"(\"after " << tableName << endl;
		return 0;
	}
	string attribute_name = getword(sql);

	s = getword(sql);
	if (s != ")") {
		cout << "Expected \")\" after " << attribute_name << endl;
		return 0;
	}
	s = getword(sql); //get ;
					  // employ api
	Index* index = new Index(tableName, indexName, attribute_name);
	api.createIndex(tableName, attribute_name, indexName);

	return 1;
}


int Interpreter::drop(string & sql) {
	string des;
	des = getword(sql);
	if (des == "index") {
		indexDrop(sql);
		return 1;
	}
	else if (des == "table") {
		tableDrop(sql);
		return 1;
	}
	else if (des == "from") {
		valueDrop(sql);
		return 1;
	}
	else {
		cout << "Expected table or index after drop." << endl;
		return 0;
	}
}

int Interpreter::indexDrop(string & sql) {
	string indexname = getword(sql);
	string s = getword(sql);
	if (s != "on") {
		cout << "expected 'on' after index name." << endl;
	}
	string tablename = getword(sql);
	s = getword(sql);
	if (s != ";") {
		cout << "expected ';' after index name." << endl;
		return 0;
	}
		
	api.dropIndex(indexname,tablename);
	return 1;
}

int Interpreter::tableDrop(string &sql) {
	string tablename = getword(sql);
	string s = getword(sql);
	if (s != ";")
		cout << "expected ';' after table name." << endl;
	api.dropTable(tablename);
	return 1;
}

int Interpreter::select(string &sql) {
	vector<string> check;//合法运算符表
	check.push_back("="); check.push_back(">"); check.push_back("<"); check.push_back("<>");
	check.push_back(">="); check.push_back("<=");
	vector<string> attr;//条件中的属性
	vector<compare> op;//条件中的运算符
	vector<string> value;//条件中的比较值
	bool isCondition = false;//是否存在where条件
	vector<string> selectAttr;
	string s = getword(sql);
	while (s != "from") {
		if (s == ",")
			s = getword(sql);
		if (s == ";") {
			cout << "syntax error, expected 'from' after attributes." << endl;
			return 0;
		}
		selectAttr.push_back(s);
		//cout << s<<endl;
		s = getword(sql);
	}
	//string selectAttr = getword(sql);
	//s = getword(sql);


	/*if (s != "from") {
	cout << "expected 'from' after attributes." << endl;
	return -1;
	}*/
	string tableName = getword(sql);
	s = getword(sql);
	if (s == "where") {
		isCondition = true;
		while (1) {
			s = getword(sql);
			attr.push_back(s);
			s = getword(sql);
			vector<string>::iterator ite;
			bool flag = false;
			for (ite = check.begin(); ite != check.end(); ++ite) {
				if (*ite == s) {
					flag = true;
					break;
				}
			}
			if (flag == true) {
				if (*ite == "=")
					op.push_back(eq);
				else if (*ite == "<")
					op.push_back(st);
				else if (*ite == ">")
					op.push_back(bt);
				else if (*ite == "<>")
					op.push_back(neq);
				else if (*ite == "<=")
					op.push_back(se);
				else if (*ite == ">=")
					op.push_back(be);
			}
			else
				cout << "expected valid operator after attributes" << endl;
			s = getword(sql);
			if (s == "and" || s == ";")
				cout << "expected value after operator" << endl;
			else {
				while (s.find("'") != s.npos)
				{
					s.erase(s.find("'"), 1);
				}
				value.push_back(s);
				//cout << s << endl;
			}
			s = getword(sql);
			if (s == ";")
				break;
			else {
				if (s != "and")
					cout << "expected ';' or 'and'." << endl;
			}

		}
	}
	if (!isCondition)
		api.select_no_condition(tableName, selectAttr);
	else {
		api.call_selection(attr, op, value, selectAttr, tableName);
	}
	//vector<string> ::iterator ite;
	//for (ite = selectAttr.begin(); ite != selectAttr.end();ite++)
	//cout <<*ite<< endl;
	//cout << tableName << endl;
	//more:call api
	return 1;
}


int Interpreter::insert(string & sql) {
	string s;
	vector<string> values;
	int count = 0;
	int total = 0;
	s = getword(sql);
	if (s != "into")
		cout << "expected 'into' after 'insert'." << endl;
	string tableName = getword(sql);
	s = getword(sql);
	if (s != "values")
		cout << "expected 'values' after 'into'." << endl;
	s = getword(sql);
	if (s != "(")
		cout << "expected '(' after 'values'." << endl;
	while (1) {
		while ((s = getword(sql)) != ")") {
			if (s == "," || s == "(")
				s = getword(sql);
			while (s.find("'") != s.npos)
			{
				s.erase(s.find("'"), 1);
			}
			values.push_back(s);
			count++;
		}
		s = getword(sql);
		if (s != ";"&&s != ",")
			cout << "expected ';' or ','." << endl;
		total++;
		if (s == ";")
			break;

	}
	api.insert(tableName, values);

	//more:call api
	return 1;

}

int Interpreter::valueDrop(string &sql) {
	string des = getword(sql);
	if (des != "from") {
		cout << "expected 'from' after 'delete'" << endl;
	}
	string tableName = getword(sql);
	string s = getword(sql);
	vector<string> attr;//条件中的属性
	vector<string> value;//条件中的值
	vector<string> check;//运算符表
	vector<compare> op;//条件中的运算符
	bool isCondition; //是否存在where条件
	check.push_back("="); check.push_back(">"); check.push_back("<"); check.push_back("<>");
	check.push_back(">="); check.push_back("<=");
	if (s != ";"&& s != "where")
		cout << "expected ';' or 'where' after tablename." << endl;
	else if (s == "where") {
		isCondition = true;
		while (1) {
			s = getword(sql);
			attr.push_back(s);
			s = getword(sql);
			vector<string>::iterator ite;
			bool flag = false;
			for (ite = check.begin(); ite != check.end(); ++ite) {
				if (*ite == s) {
					flag = true;
					break;
				}
			}
			if (flag == true) {
				if (*ite == "=")
					op.push_back(eq);
				else if (*ite == "<")
					op.push_back(st);
				else if (*ite == ">")
					op.push_back(bt);
				else if (*ite == "<>")
					op.push_back(neq);
				else if (*ite == "<=")
					op.push_back(se);
				else if (*ite == ">=")
					op.push_back(be);
			}

			else
				cout << "expected valid operator after attributes" << endl;
			s = getword(sql);
			if (s == "and" || s == ";")
				cout << "expected value after operator" << endl;
			else {
				while (s.find("'") != s.npos)
				{
					s.erase(s.find("'"), 1);
				}
				value.push_back(s);
				//cout << s << endl;
			}
			s = getword(sql);
			if (s == ";")
				break;
			else {
				if (s != "and")
					cout << "expected ';' or 'and'." << endl;
			}

		}
	}
	if (attr.size() == 0)
		api.call_deletion_no_condition(tableName);
	else api.call_deletion_condiion(attr, op, value, tableName);
	//call api
	return 1;
}

int	Interpreter::quit() {
	//把所有manager析构掉？ more：call API
	api.quit();
	return 1;
}



int Interpreter::execfile(string &sql) {
	string filename = getword(sql);
	fstream  fin(filename, ios::in);
	vector<string> commandList;
	int flag = false;
	while (fin.good()) {
		string s;
		string fsql;
		fin >> s;
		if (s == "" && !fin.good())
			break;
		fsql += s;
		fsql += " ";
		while (s.find(';') == s.npos) {
			fin >> s;
			
			fsql += s;
			fsql += " ";
		}
		commandList.push_back(fsql);
	}
	if (commandList.size() > 3000) {
		string s1 = regular_SQL(commandList[0]);
		flag = true;
		runSQL(s1);
		catalogmanager cm;
		string sql = commandList[0];
		getword(sql);
		getword(sql);
		string tablename = getword(sql);
		for (int i = 0; i < cm.tableList.size(); i++) {
			if (cm.tableList[i].name == tablename) {
				for (int j = 0; j < cm.tableList[i].attr.size(); j++) {
					if (cm.tableList[i].attr[j].isUnique && cm.tableList[i].attr[j].index == "empty") {
						cm.tableList[i].attr[j].index = cm.tableList[i].attr[j].name;
						int type = cm.tableList[i].attr[j].type;
						if (type == INT_TYPE) {
							BPlusTree<int> * bt = new BPlusTree<int>(cm.tableList[i].name, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
							bt->createTree();
							Index index(cm.tableList[i].name,cm.tableList[i].attr[j].index,cm.tableList[i].attr[j].name);
							cm.indexList.push_back(index);
							api.bt_int.push_back(bt);
						}
						else if (type == FLOAT_TYPE) {
							BPlusTree<float> * bt = new BPlusTree<float>(cm.tableList[i].name, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
							bt->createTree();
							Index index(cm.tableList[i].name, cm.tableList[i].attr[j].index, cm.tableList[i].attr[j].name);
							cm.indexList.push_back(index);
							api.bt_float.push_back(bt);
						}
						else {
							BPlusTree<string> * bt = new BPlusTree<string>(cm.tableList[i].name, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
							bt->createTree();
							Index index(cm.tableList[i].name, cm.tableList[i].attr[j].index, cm.tableList[i].attr[j].name);
							cm.indexList.push_back(index);
							api.bt_string.push_back(bt);
						}
						
					}
				}
			}
		}
	}
	vector<string>::iterator ite;
	for (ite = commandList.begin(); ite != commandList.end(); ++ite) {

		if (ite == commandList.begin() && flag)
			continue;
		string s1 = regular_SQL(*ite);
		runSQL(s1);

	}
	return 1;
}


//int main() {
//	Interpreter * it = new Interpreter();
//	string s = it->getCommand();
//	string s1 = it->regular_SQL(s);
//	cout << s1;
//	it->runSQL(s1);
//	getchar();
//	getchar();
//}




