#include"API.h"
#include<string>
#include<iostream>
#include "MiniSQL.h"
#include <iomanip>
using namespace std;

bool API :: judge_index(string tablename) {
	for (int i = 0; i < bt_int.size(); i++) {
		if (bt_int[i]->tableName == tablename) {
			return true;
		}
	}
	for (int i = 0; i < bt_float.size(); i++) {
		if (bt_float[i]->tableName == tablename) {
			return true;
		}
	}
	for (int i = 0; i < bt_string.size(); i++) {
		if (bt_string[i]->tableName == tablename) {
			return true;
		}
	}
	return false;
}

void API::findBPlusTree(string tablename) {
	catalogmanager cm;
	for (int i = 0; i < cm.tableList.size(); i++) {
		if (cm.tableList[i].name == tablename) {
			for (int j = 0; j < cm.tableList[i].attr.size(); j++) {
				if (cm.tableList[i].attr[j].index != "empty") {
					int type = cm.tableList[i].attr[j].type;
					if (type == INT_TYPE) {
						BPlusTree<int>* bt = new BPlusTree<int>(tablename, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
						bt_int.push_back(bt);
					}
					else if (type == FLOAT_TYPE)
					{
						BPlusTree<float>* bt = new BPlusTree<float>(tablename, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
						bt_float.push_back(bt);
					}
					else
					{
						BPlusTree<string>* bt = new BPlusTree<string>(tablename, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
						bt_string.push_back(bt);
					}
				}
			}
			break;
		}
	}
}

void API::quit() {
	rm.ob.flushAll();
	for (int i = 0; i < bt_int.size(); i++) {
		bt_int[i]->writeTree();
	}
	for (int i = 0; i < bt_float.size(); i++) {
		bt_float[i]->writeTree();
	}
	for (int i = 0; i < bt_string.size(); i++) {
		bt_string[i]->writeTree();
	}
	cout <<"bye!" <<endl;
	exit(0);
}

void print_result(turples tur, Table table, vector<string> selectAttr);
void API::call_selection(vector<string> &attr, vector<compare> &op, vector<string> &value, vector<string> &selectAttr,string &tablename)
{
	if (!judge_index(tablename)) {
		findBPlusTree(tablename);
	}
	vector<comparision_condition> temp_noindex;
	vector<comparision_condition> temp_index;
	comparision_condition tmp_noindex;
	comparision_condition tmp_index;
 	Table table;
	turples tur;
	catalogmanager cm;
	vector<string> output_temp;

	bool table_exist = false;
	for (int j = 0; j < cm.tableList.size(); j++)
	{
		if (tablename == cm.tableList[j].name)
		{
			table = cm.tableList[j];
			table_exist = true;
			break;
		}
	} //找到table

	if (table_exist == false)
	{
		cout << "the table which is selecting does not exist" << endl;
		return;
	}
	for (int i = 0; i < attr.size(); i++)
	{
		bool attr_exist = false, attr_exist_index = false;
		for (int j = 0; j < table.attr.size(); j++)
		{
			if (attr[i] == table.attr[j].name)
			{
				attr_exist = true;
				if (table.attr[j].index != "empty")
				{
					
					tmp_index.value = value[i];
					tmp_index.condition = op[i];
					tmp_index.columnnum = j;
					attr_exist_index = true;
				}
				else
				{
					tmp_noindex.value = value[i];
					tmp_noindex.condition = op[i];
					tmp_noindex.columnnum = j;
				}
			}
		}//找到where中的属性
		if (!attr_exist)
			cout << "the attribute does not exist in the table" << endl;
		if (attr_exist_index)
			temp_index.push_back(tmp_index);
		else
			temp_noindex.push_back(tmp_noindex);
	}
	//分别找到有index的搜索条件和没有index的搜索条件存储在不同conditons中

	tur= rm.selection(table,temp_noindex,temp_index,bt_int,bt_float,bt_string);
	print_result(tur, table, selectAttr);
}

void print_result(turples tur, Table table, vector<string> selectAttr)
{
	int *point = new int[table.attr.size()];
	int size = 0;

	if (tur.values.size() == 0)
	{
		cout << "Query OK! " << tur.values.size() << " rows selected!" << endl;
		return;
	}
	cout << "------------------------------------" << endl;


	if (selectAttr[0] == "*")
	{
		for (int j = 0; j < table.attr.size(); j++)
		{
			point[size] = j;
			size++;
		}
	}
	else
	{
		for (int j = 0; j < selectAttr.size(); j++)                    //第j的投影属性
		{
			for (int k = 0; k < table.attr.size(); k++)                //第k个属性
			{
				if (selectAttr[j] == table.attr[k].name)
				{
					point[size] = k;
					size++;
					break;
				}
			}
		}//投影
	}
	if (tur.values.size() != 0) {
		for (int k = 0; k < size; k++)
		{
			cout << setw(14) << setiosflags(ios::left) << table.attr[point[k]].name << setw(10);
		}
		cout << endl;
	}
	cout << "------------------------------------" << endl;
	for (int j = 0; j < tur.values.size(); j++)                 //多少个turple
	{
		for (int k = 0; k < size; k++)
		{
			cout << setw(14) << setiosflags(ios::left) << tur.values[j].row_value[point[k]]  << setw(14);
		}
		cout << endl;
	}
	cout << "Query OK! " << tur.values.size() << " rows selected!" << endl;
}

void API::createTable(Table *t,string tablename,vector<Attribute> attrs) {
	catalogmanager cm;
	for (int i = 0; i < t->attr.size(); i++) {
		if (t->attr[i].index != "empty") {
			if (t->attr[i].type == INT_TYPE) {
				BPlusTree<int> *bt = new BPlusTree<int>(t->name, t->attr[i].name, t->attr[i].index);
				bt->createTree();
				bt->writeTree();
				bt_int.push_back(bt);
			}
			else if (t->attr[i].type == FLOAT_TYPE) {
				BPlusTree<float>* bt = new BPlusTree<float>(t->name, t->attr[i].name, t->attr[i].index);
				bt->createTree();
				bt->writeTree();
				bt_float.push_back(bt);
			}
			else {
				BPlusTree<string>* bt= new BPlusTree<string> (t->name, t->attr[i].name, t->attr[i].index);
				bt->createTree();
				bt->writeTree();
				bt_string.push_back(bt);
			}
		}
	}
	
	if (cm.createTable(tablename, attrs)) {
		rm.create(*t);
		cout << "Query OK ! table created!" << endl;
	}
	else 
	{
		cout << "Table '" + tablename + "' already exists" << endl;
	}
}

void API::dropIndex(string indexname,string tablename) {
	catalogmanager cm;
	int type = -1;
	if (cm.dropindex(indexname,tablename)) {
		for (int i = 0; i < cm.tableList.size(); i++) {
			if (cm.tableList[i].name == tablename) {
				for (int j = 0; j < cm.tableList[i].attr.size(); j++) {
					if (cm.tableList[i].attr[j].index == indexname) {
						type = cm.tableList[i].attr[j].type;
						break;
					}
				}
				break;
			}
		}
		if (type == INT_TYPE) {
			for (vector<BPlusTree<int>*>::iterator it = bt_int.begin(); it != bt_int.end();) {
				if (indexname == (*it)->indexName && tablename == (*it)->tableName) {
					it = bt_int.erase(it);
				}
				else
					it++;
			}

		}
		else if (type == FLOAT_TYPE) {
			for (vector<BPlusTree<float>*>::iterator it = bt_float.begin(); it != bt_float.end();) {
				if (indexname == (*it)->indexName && tablename == (*it)->tableName) {
					it = bt_float.erase(it);
				}
				else
					it++;

			}
		}
		else {
			for (vector<BPlusTree<string>*>::iterator it = bt_string.begin(); it != bt_string.end();) {
				if (indexname == (*it)->indexName && tablename == (*it)->tableName) {
					it = bt_string.erase(it);
				}
				else
					it++;
			}
		}
		cout << "Query OK! 0 row affected!" << endl;
	}
	else
	{
		cout << "Can't drop " + indexname + " ; check that index exists" << endl;
	}
	
}
void API::createIndex(string tablename, string attr_name, string indexname) {
	catalogmanager cm;
	cm.createIndex(tablename, indexname, attr_name);
	Table tb;
	int i;
	for (i = 0; i < cm.tableList.size(); i++) {
		if (cm.tableList[i].name == tablename) {
			tb = cm.tableList[i];
			break;
		}
	}
	if (i == cm.tableList.size()) {
		cout << "The table you want to select from does not exist!" << endl;
		return;
	}
	turples  result;
	vector<record> rec;
	rm.selection(tb, rec, result);
	for (i = 0; i < tb.attr.size(); i++) {
		if (tb.attr[i].name == attr_name) {
			break;
		}
	}
	if (i == tb.attr.size()) 
	{
		cout << "Error: Attribute '" + attr_name + +"' does not exist,check it" << endl;
		return;
	}
	if (tb.attr[i].type == INT_TYPE)
	{
		BPlusTree<int> * bt = new BPlusTree<int>(tablename, attr_name, indexname);
		bt->createTree();
		for (int k = 0; k < result.values.size(); k++)
		{
			int key = stoi(result.values[k].row_value[i]);
			bt->insertKey(key, rec[k]);
		}
		bt->writeTree();
		bt_int.push_back(bt);
	}
	else if (tb.attr[i].type == FLOAT_TYPE)
	{
		BPlusTree<float> * bt = new BPlusTree<float>(tablename, attr_name, indexname);
		bt->createTree();
		for (int k = 0; k < result.values.size(); k++)
		{
			float key = stof(result.values[k].row_value[i]);
			bt->insertKey(key, rec[k]);
		}
		bt->writeTree();
		bt_float.push_back(bt);
	}
	else
	{
		BPlusTree<string> * bt = new BPlusTree<string>(tablename, attr_name, indexname);
		bt->createTree();
		for (int k = 0; k < result.values.size(); k++)
		{
			string key = (result.values[k].row_value[i]);
			bt->insertKey(key, rec[k]);
		}
		bt->writeTree();
		bt_string.push_back(bt);
	}

	cout << "Query OK! Index created " << endl;
}
void API::dropTable(string tablename) {
	catalogmanager cm;
	for (vector<BPlusTree<int>*>::iterator it = bt_int.begin(); it != bt_int.end();) {
		if (tablename == (*it)->tableName) {
			it = bt_int.erase(it);
		}
		else {
			++it;
		}
	}
	
	for (vector<BPlusTree<float>*>::iterator it = bt_float.begin(); it != bt_float.end();) {
		if ((*it)->tableName == tablename)
			it = bt_float.erase(it);
		else
			it++;
	}
	for (vector<BPlusTree<string>*>::iterator it = bt_string.begin(); it != bt_string.end();) {
		if ((*it)->tableName == tablename)
			it = bt_string.erase(it);
		else
			it++;
	}
	if (cm.droptable(tablename))
	{
		cout << "Query OK! Table dropped! " << endl;
	}
	else
	{
		cout << "Error : Unknown table '" + tablename + "'" << endl;
	}
	
}
void API::insert(string tablename,vector<string> s) {
	catalogmanager cm;
	
	int i;
	turple a;
	a.row_value = s;
	if (!judge_index(tablename)) {
		findBPlusTree(tablename);
	}
	for (i = 0; i < cm.tableList.size(); i++) {
		if (cm.tableList[i].name == tablename) {
			record rec;
			if (!rm.insertion(cm.tableList[i], a, rec,bt_int,bt_float,bt_string))
				return;
			for (int j = 0; j < cm.tableList[i].attr.size(); j++) {
				if (cm.tableList[i].attr[j].index != "empty") {
					if (cm.tableList[i].attr[j].type == INT_TYPE) {
						int key = stoi(s[j]);
						for (int m = 0; m < bt_int.size(); m++) {
							if (cm.tableList[i].attr[j].index == bt_int[m]->indexName &&cm.tableList[i].attr[j].name == bt_int[m]->attrName && cm.tableList[i].name == bt_int[m]->tableName)
							{
								bt_int[m]->insertKey(key, rec);
								//bt_int[m]->writeTree();
							}
						}
						/*BPlusTree<int> * bt = new BPlusTree<int> (cm.tableList[i].name,cm.tableList[i].attr[j].name,cm.tableList[i].attr[j].index);
						bt->insertKey(key, rec);
						bt->writeTree();
						delete bt;*/
						/*if (bt) 
							delete bt;*/
					}
					else if (cm.tableList[i].attr[j].type == FLOAT_TYPE) {
						float key = stof(s[j]);
						for (int m = 0; m < bt_float.size(); m++) {
							if (cm.tableList[i].attr[j].index == bt_float[m]->indexName &&cm.tableList[i].attr[j].name == bt_float[m]->attrName && cm.tableList[i].name == bt_float[m]->tableName)
							{
								bt_float[m]->insertKey(key, rec);
								//bt_float[m]->writeTree();
							}
						}
						/*BPlusTree<float> * bt = new BPlusTree<float> (cm.tableList[i].name, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
						bt->insertKey(key, rec);
						bt->writeTree();*/
					}
					else {
						string key = s[j];
						for (int m = 0; m < bt_string.size(); m++) {
							if (cm.tableList[i].attr[j].index == bt_string[m]->indexName &&cm.tableList[i].attr[j].name == bt_string[m]->attrName && cm.tableList[i].name == bt_string[m]->tableName)
							{
								bt_string[m]->insertKey(key, rec);
								//bt_string[m]->writeTree();
							}
						}
						/*BPlusTree<string> *bt = new BPlusTree<string>(cm.tableList[i].name, cm.tableList[i].attr[j].name, cm.tableList[i].attr[j].index);
						bt->insertKey(key, rec);
						bt->writeTree();*/
					}
				}
			}
			break;
		}
	}
	
	if (i  == cm.tableList.size()) {
		cout << "The table you want to insert does not exist!" << endl;
		return;
	}
}

void API::select_no_condition(string tablename, vector<string> select_Attr) {
	catalogmanager cm;
	Table tb;
	int i;
	for (i = 0; i < cm.tableList.size(); i++) {
		if (cm.tableList[i].name == tablename) {
			tb = cm.tableList[i];
			break;
		}
	}
	if (i == cm.tableList.size()) {
		cout << "The table you want to select from does not exist!" <<endl;
		return;
	}
	turples  result = rm.selection(tb);
	print_result(result, tb, select_Attr);
}


void API::call_deletion_no_condition(string &table)
{
	catalogmanager cm;
	Table tb;
	int count;
	int result;
	

	for (count = 0; count < cm.tableList.size(); count++) {
		if (cm.tableList[count].name == table) {
			result = cm.tableList[count].recordnum;
			break;
		}
	}
	//找到tablelist中对应的table
	if (count == cm.tableList.size())
	{
		cout << "The table you want to delete from does not exist!" << endl;
		return;
	}
	turples rows = rm.selection(cm.tableList[count]);

	for (int k = 0; k < rows.values.size(); k++)//对每一个turple
	{
		turple row = rows.values[k];
		vector<string> s = row.row_value;
		for (int j = 0; j < cm.tableList[count].attr.size(); j++) {         //对每一个属性
			if (cm.tableList[count].attr[j].index != "empty") {
				if (cm.tableList[count].attr[j].type == INT_TYPE) {
					int key = stoi(s[j]);
					for (int m = 0; m < bt_int.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_int[m]->indexName &&cm.tableList[count].attr[j].name == bt_int[m]->attrName && cm.tableList[count].name == bt_int[m]->tableName)
						{
							bt_int[m]->deleteKey(key);
							bt_int[m]->writeTree();
						}
					}
					/*BPlusTree<int> * bt = new BPlusTree<int>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
				else if (cm.tableList[count].attr[j].type == FLOAT_TYPE) {
					float key = stof(s[j]);
					for (int m = 0; m < bt_float.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_float[m]->indexName &&cm.tableList[count].attr[j].name == bt_float[m]->attrName && cm.tableList[count].name == bt_float[m]->tableName)
						{
							bt_float[m]->deleteKey(key);
							bt_float[m]->writeTree();
						}
					}
					/*BPlusTree<float> * bt = new BPlusTree<float>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
				else {
					string key = s[j];
					for (int m = 0; m < bt_string.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_string[m]->indexName &&cm.tableList[count].attr[j].name == bt_string[m]->attrName && cm.tableList[count].name == bt_string[m]->tableName)
						{
							bt_string[m]->deleteKey(key);
							bt_string[m]->writeTree();
						}
					}
					/*BPlusTree<string> * bt = new BPlusTree<string>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
			}
		}
	}

	rm.deletion(cm.tableList[count]);
	cout << '\t' << "Query OK! " << result << " rows affected!" << endl;
}


void API::call_deletion_condiion(vector<string> &attr, vector<compare> &op, vector<string> &value, string &tablename)
{
	if (!judge_index(tablename)) {
		findBPlusTree(tablename);
	}
	vector<comparision_condition> temp_noindex;
	vector<comparision_condition> temp_index;
	vector<comparision_condition> all;
	comparision_condition tmp_noindex;
	comparision_condition tmp_index;
 	Table table;
	turples tur;
	catalogmanager cm;
	vector<string> output_temp;
	int count;
	bool table_exist = false;
	for (int j = 0; j < cm.tableList.size(); j++)
	{
		if (tablename == cm.tableList[j].name)
		{
			table = cm.tableList[j];
			table_exist = true;
			count = j;
			break;
		}
	} //找到table

	if (table_exist == false) {
		cout << "Error : Table '" + tablename + "' does not exist, check it " << endl;
		return;
	}
		
	for (int i = 0; i < attr.size(); i++)
	{
		bool attr_exist = false, attr_exist_index = false;
		for (int j = 0; j < table.attr.size(); j++)
		{
			if (attr[i] == table.attr[j].name)
			{
				attr_exist = true;
				if (table.attr[j].index != "empty")
				{
					
					tmp_index.value = value[i];
					tmp_index.condition = op[i];
					tmp_index.columnnum = j;
					attr_exist_index = true;
				}
				else
				{
					tmp_noindex.value = value[i];
					tmp_noindex.condition = op[i];
					tmp_noindex.columnnum = j;
				}
			}
		}//找到where中的属性
		if (!attr_exist)
		{
			cout << "Error: Attribute does not exist, check it" << endl;
			return;
		}
			

		if (attr_exist_index)
		{
			temp_index.push_back(tmp_index);
			all.push_back(tmp_index);
		}
		else
		{
			temp_noindex.push_back(tmp_noindex);
			all.push_back(tmp_noindex);
		}
			
	}
	//分别找到有index的搜索条件和没有index的搜索条件存储在不同conditons中

	tur= rm.selection(table,temp_noindex,temp_index, bt_int, bt_float, bt_string);
	int result = tur.values.size();
	rm.deletion(cm.tableList[count],temp_noindex,temp_index, bt_int, bt_float, bt_string);
	for (int k = 0; k < tur.values.size(); k++)//对每一个turple
	{
		turple row = tur.values[k];
		vector<string> s = row.row_value;
		for (int j = 0; j < cm.tableList[count].attr.size(); j++) {         //对每一个属性
			if (cm.tableList[count].attr[j].index != "empty") {
				if (cm.tableList[count].attr[j].type == INT_TYPE) {
					int key = stoi(s[j]);
					for (int m = 0; m < bt_int.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_int[m]->indexName &&cm.tableList[count].attr[j].name == bt_int[m]->attrName && cm.tableList[count].name == bt_int[m]->tableName)
						{
							bt_int[m]->deleteKey(key);
							bt_int[m]->writeTree();
						}
					}
					/*BPlusTree<int> * bt = new BPlusTree<int>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
				else if (cm.tableList[count].attr[j].type == FLOAT_TYPE) {
					float key = stof(s[j]);
					for (int m = 0; m < bt_float.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_float[m]->indexName &&cm.tableList[count].attr[j].name == bt_float[m]->attrName && cm.tableList[count].name == bt_float[m]->tableName)
						{
							bt_float[m]->deleteKey(key);
							bt_float[m]->writeTree();
						}
					}
					/*BPlusTree<float> * bt = new BPlusTree<float>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
				else {
					string key = s[j];
					for (int m = 0; m < bt_string.size(); m++) {
						if (cm.tableList[count].attr[j].index == bt_string[m]->indexName &&cm.tableList[count].attr[j].name == bt_string[m]->attrName && cm.tableList[count].name == bt_string[m]->tableName)
						{
							bt_string[m]->deleteKey(key);
							bt_string[m]->writeTree();
						}
					}
					/*BPlusTree<string> * bt = new BPlusTree<string>(cm.tableList[count].name, cm.tableList[count].attr[j].name, cm.tableList[count].attr[j].index);
					bt->deleteKey(key);
					bt->writeTree();*/
				}
			}
		}
	}


	cout << "Query OK! " << tur.values.size() << " rows selected!" << endl;
}