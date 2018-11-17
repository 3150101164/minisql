#ifndef API_H
#define API_H
#include<iostream>
#include"catalogmanager.h"
#include"recordmanager.h"
#include"IndexManager.h"
class API
{
public:
	//catalogmanager cm;
	record_management rm;
	vector<BPlusTree<int>*> bt_int;
	vector<BPlusTree<float>*> bt_float;
	vector<BPlusTree<string>*> bt_string;
	API(){}
	~API(){}
	void call_selection(vector<string> &attr, vector<compare> &op, vector<string> &value, vector<string> &selectAttr,string &tablename);
	//输入参数分别为选择的三大条件、投影的属性、table
	void quit();
	void createTable(Table *t,string tablename,vector<Attribute> attrs);
	void dropIndex(string,string);
	void dropTable(string);
	void createIndex(string, string, string);
	void insert(string tablename, vector<string>) ;
	void select_no_condition(string tablename, vector<string> select_Attr);
	void call_deletion_no_condition(string &table);
	void call_deletion_condiion(vector<string> &attr, vector<compare> &op, vector<string> &value, string &tablename);
	void findBPlusTree(string tablename);
	bool judge_index(string tablename);
};



#endif // !API_H

