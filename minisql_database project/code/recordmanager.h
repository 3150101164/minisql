#ifndef RECORD1
#define RECORD1

#include<iostream>
#include<string>
#include<vector>
#include"MiniSQL.h"
#include"BufferManager.h"
#include  "IndexManager.cpp"


using namespace std;
/*
class Attribute
{
public:
	string name;
	bool ip;       //是不是主键
	bool iu;       //是不是unique
	int length;
	int type;     //1是int 2是char 3是float
	Attribute()
	{
		ip = false;
		iu = false;
	}
	Attribute(string a,bool b,bool c,int d,int e):name(a),ip(b),iu(c),length(d),type(e){}
	~Attribute(){}
};
*/
class turple
{
public:
	vector<string> row_value;
};

class turples
{
public:
	vector<turple> values;
};//一个turple



/*
class Table
{
public:
	string name;
	int attrnum;
	int blocknum;
	vector<Attribute> Attributes;
	Table(string a,int b=0,int c=0):name(a),attrnum(b),blocknum(c){}
	Table(){}
	~Table(){}
};
*/
//enum compare{bt,be,st,se,eq,neq};//bigger than,bigger than or equal,smaller than,smaller than and equal,equal,not equal
class comparision_condition
{
public:
	compare condition;
	string value;
	int columnnum;  //第几列，哪一个属性,为统一，第一个属性定义为第0个属性
	comparision_condition(){}
	comparision_condition(compare c, string v, int col) {
		condition = c;
		value = v;
		columnnum = col;
	}
	~comparision_condition(){}
};

//extern BufferManager ob;
class record_management
{
private:
	turple spilt_turple(Table table, string row);
	char*  join_turple(Table table, turple &a);
	bool compare_function(Table table,vector<comparision_condition> conditions,turple &row );
	turple char2turple(Table table, char*s);
public:
	record_management(){}
	BufferManager ob;
	//~record_management(){}
	turples selection(Table &table, vector<comparision_condition> &conditions, vector<comparision_condition> &s,vector<BPlusTree<int>*>&, vector<BPlusTree<float>*>&, vector<BPlusTree<string>*>&);//执行选择语句操作，conditions代表选择select语句的要求
	turples selection(Table &table);//将table中所有数据输出，即select *
	bool insertion(Table &table, turple &a, record &rec, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string);
	void deletion(Table &table,vector<comparision_condition> conditions,vector<comparision_condition>conditions_index, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string);//delete from 同上
	void deletion(Table &table);//delete全部
	void selection(Table &table, vector<record> &rec, turples &result);
	void drop(Table &table);//drop table,输入table变量即可
	void create(Table &table);//create table
	void display_turples(turples&a);//提供display接口使得能显示所select的turples;
};
//enum compare{bt,be,st,se,eq,neq};//bigger than,bigger than or equal,smaller than,smaller than and equal,equal,not equal
//attention:输入表格属性时，记得增加record__length,我现在不知道在哪加比较好实在要加的话，随便找个地方加个接口就好.

#endif
