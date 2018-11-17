#ifndef _MINISQL_H_
#define _MINISQL_H_
#define INT_TYPE 1
#define CHAR_TYPE 2
#define FLOAT_TYPE 3
#include<stdio.h>
#include <string>
#include<iostream>
#include<vector>
using namespace std;
enum compare { bt, be, st, se, eq, neq };
class record {
public:
	string tableName;
	int block_offset;
	int content_offset;
	int length;
	record(string tb, int bo, int co, int len) {
		tableName = tb;
		block_offset = bo;
		content_offset = co;
		length = len;
	}
	record(){}
	~record() {}
};
class Attribute {
public:
	int type;
	int length;
	string name;
	bool isUnique;
	bool isPrimary;
	string index;
	Attribute() :name(""), index(""), isUnique(false), isPrimary(false), type(INT_TYPE), length(0) {}

	Attribute(string name, bool isUnique, bool isPrimary, string index) {
		this->name = name;
		this->isUnique = isUnique;
		this->index = index;
		this->isPrimary = isPrimary;
	}
	Attribute(string name, bool isUnique = false, bool isPrimary = false, string index = "", int type = 1, int length = 0) {
		this->name = name;
		this->isUnique = isUnique;
		this->index = index;
		this->isPrimary = isPrimary;
		this->type = type;
		this->length = length;
	}
};
class Table
{
public:
	string name;   //all the datas is store in file name.table
	int recordnum;	//number of block the datas of the table occupied in the file name.table
	int recordLength;	//total length of one record, should be equal to sum(attributes[i].length)
	vector<Attribute> attr;
	Table() {}
	Table(string name, vector<Attribute> attri, int rn) :name(name), attr(attri), recordnum(rn) {
		this->recordLength = 0;
		vector<Attribute>::iterator ite;
		for (ite = this->attr.begin(); ite != this->attr.end(); ite++) {
			if (ite->type == INT_TYPE) {
				this->recordLength += 4;
			}
			else if (ite->type == FLOAT_TYPE) {
				this->recordLength += 4;
			}
			else if (ite->type == CHAR_TYPE) {
				this->recordLength += ite->length;
			}
		}
	}
	//Table(string tbname, int attrinum, int blocknum, int  )
	friend ostream& operator<<(ostream& out, const Table& node)
	{
	out << node.name << '\t' << node.recordnum << '\t' << node.recordLength <<'\t'<<node.attr.size()<<endl;
	vector<Attribute>::const_iterator ite;
	for( ite = node.attr.begin();ite != node.attr.end();++ite){
		out << ite->name << '\t' << ite->type << '\t' << ite->length << '\t' << ite->isUnique << '\t' << ite->isPrimary << '\t' << ite->index << endl;
	}

	return out;
	}

	friend istream& operator>>(istream& in, Table& node)
	{
	in >> node.name >> node.recordnum>>node.recordLength;
	int s;
	in >> s;
	if (s < 0)
	return in;
	node.attr.resize(s);
	for (vector<Attribute>::iterator aite = node.attr.begin(); aite != node.attr.end(); ++aite)
	{
	in >> aite->name >> aite->type >> aite->length >> aite->isUnique >> aite->isPrimary >> aite->index ;
	}
	return in;
	}
};

class Index
{
public:
	string indexName;
	string tbname;
	//int column;
	string attrname;
	//int blocknum;
	Index() {}
	Index(string tbn, string indexname, string attrn) :
		indexName(indexname), tbname(tbn), attrname(attrn) {

	}
	friend ostream& operator<<(ostream& out, const Index& node)
	{
	out << node.tbname << '\t' << node.indexName << '\t' << node.attrname <<  endl;
	return out;
	}

	friend istream& operator>>(istream& in, Index& node)
	{
	in >> node.tbname >> node.indexName >> node.attrname;
	return in;
	}
};




#endif

