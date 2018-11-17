#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H
#define MAX 100
#define MAX_NODE 500
#include <string>
#include "MiniSQL.h"
#include <iostream>
#include "BufferManager.h"
#include <cstring>
#include <stdlib.h>
using namespace std;

template <class T>
class BPlusNode {
public:
	T key[MAX];
	int pointer[MAX + 1];
	int nkey;
	bool isleaf;
	bool isroot;
	int father;
	
	record  record_pointer[MAX];
	BPlusNode(bool il,bool is) {
		isleaf = il;
		isroot = is;
		nkey = 0;
		father = -1;
		for (int i = 0; i < MAX + 1; i++)
			pointer[i] = -1;
	}
	~BPlusNode(){}
	BPlusNode() {
		isleaf = 0;
		isroot = 0;
		nkey = 0;
		father = -1;
		for (int i = 0; i < MAX + 1; i++)
			pointer[i] = -1;
		/*if (typeid(T) == typeid(int) || typeid == typeid(float))
			for (int i = 0; i < MAX; i++)
				key[i] = 0;
		else if(typeid(T) == typeid(string))
			for (int i = 0; i < MAX; i++) {
				key[i] = "";
			}*/
	}
};

template <class T>

class BPlusTree {
public:
	string fileName;
	string attrName;
	string tableName;
	string indexName;
	int nNode;
	int data_index;

	int root_index;
	BPlusNode<T> Nodes[MAX_NODE];

	void insertKey(T key,record r);
	void insertKey(T key);
	void deleteKey(T key);
	record* searchKey(T key);
	void deletekeyfromLeafNode(T key, BPlusNode<T> * bt);
	void deletekeyfromInterNode(T key, BPlusNode<T>  * bt);
	void writeTree();
	void  readTree();
	vector<record>  findRecords(compare c, T key);
	void createTree();
	void splitBplusNode(int current_index, const int childnum);
	int newBplusNode();
	BPlusTree(string tablename,string att, string index){
		attrName = att;
		indexName = index;
		tableName = tablename;
		nNode = 0;
		data_index = 0;
		this->readTree();


	}
	~BPlusTree() {
		writeTree();
		cout << "niubi" << endl;
	}
	BPlusTree() {
	}

};

#endif // 
