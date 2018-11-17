#ifndef _catalogmanager_H_
#define _catalogmanager_H_

#include "MiniSQL.h"
//#include<indexNode.h>
#include "IndexManager.h"
#include<vector>
#include<string>
#include<cstdio>  
#include<list>


class catalogmanager {
public:
	vector<Table> tableList;
	vector<Index> indexList;

public:
	catalogmanager();
	virtual ~catalogmanager();
	void readTableCat();
	void writeTableCat();
	void readIndexCat();
	void writeIndexCat();
	bool createTable(string tablenm, vector<Attribute> attrlist);
	bool createIndex(string tablenm, string indexnm, string attrnm);
	bool isExistindex(string indexname, string tablenm);
	bool isExisttable(string tablenm);
	bool droptable(string tablenm);
	bool dropindex(string ,string);
	bool alterRecord(string tablenm, int num);
	bool addAttribute(string tablenm, Attribute newAttr);
	bool deleteAttribute(string tablenm, string oldAttr);

};

#endif







