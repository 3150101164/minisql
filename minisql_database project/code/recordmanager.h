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
	bool ip;       //�ǲ�������
	bool iu;       //�ǲ���unique
	int length;
	int type;     //1��int 2��char 3��float
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
};//һ��turple



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
	int columnnum;  //�ڼ��У���һ������,Ϊͳһ����һ�����Զ���Ϊ��0������
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
	turples selection(Table &table, vector<comparision_condition> &conditions, vector<comparision_condition> &s,vector<BPlusTree<int>*>&, vector<BPlusTree<float>*>&, vector<BPlusTree<string>*>&);//ִ��ѡ����������conditions����ѡ��select����Ҫ��
	turples selection(Table &table);//��table�����������������select *
	bool insertion(Table &table, turple &a, record &rec, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string);
	void deletion(Table &table,vector<comparision_condition> conditions,vector<comparision_condition>conditions_index, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string);//delete from ͬ��
	void deletion(Table &table);//deleteȫ��
	void selection(Table &table, vector<record> &rec, turples &result);
	void drop(Table &table);//drop table,����table��������
	void create(Table &table);//create table
	void display_turples(turples&a);//�ṩdisplay�ӿ�ʹ������ʾ��select��turples;
};
//enum compare{bt,be,st,se,eq,neq};//bigger than,bigger than or equal,smaller than,smaller than and equal,equal,not equal
//attention:����������ʱ���ǵ�����record__length,�����ڲ�֪�����ļӱȽϺ�ʵ��Ҫ�ӵĻ�������Ҹ��ط��Ӹ��ӿھͺ�.

#endif
