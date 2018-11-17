#include "MiniSQL.h"
#include <string>
#include <list>
#include<vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include<algorithm>
#include"catalogmanager.h"
using namespace std;
catalogmanager::catalogmanager()
{
	readTableCat();
	readIndexCat();
}

catalogmanager::~catalogmanager()
{
	writeTableCat();
	writeIndexCat();
}

void catalogmanager::readTableCat() {
	const string filename = "table.catlog";
	fstream  fin;
	fin.open(filename, ios::in);
	if (fin.good()) {	
		copy(istream_iterator<Table>(fin), istream_iterator<Table>(), back_inserter(this->tableList));
		fin.close();
	}

}

void catalogmanager::readIndexCat() {
	const string filename = "index.catlog";
	fstream  fin(filename.c_str(), ios::in);
	if (fin.good()) {
		copy(istream_iterator<Index>(fin), istream_iterator<Index>(), back_inserter(this->indexList));
		fin.close();
	}
}


void catalogmanager::writeIndexCat() {
	const string filename = "index.catlog";
	fstream  fout(filename.c_str(), ios::out);
	if (fout.good()) {
		copy(this->indexList.begin(), this->indexList.end(), ostream_iterator<Index>(fout));
		fout.close();
	}
}

void catalogmanager::writeTableCat() {
	const string filename = "table.catlog";
	fstream  fout(filename.c_str(), ios::out);
	if (fout.good()) {
		copy(this->tableList.begin(), this->tableList.end(), ostream_iterator<Table>(fout));
		fout.close();
	}
}


bool catalogmanager::createTable(string tablenm, vector<Attribute> attrlist) {
	if (isExisttable(tablenm) == true) {
		return false;
	}

	Table node(tablenm, attrlist, 0);
	vector <Attribute>::iterator it;
	this->tableList.push_back(node);
	for (it = attrlist.begin(); it != attrlist.end(); it++) {
		if (it->isPrimary){
			Index  ind(tablenm,it->name,it->name);
			this->indexList.push_back(ind);
		}
	}
	/*writeIndexCat();
	writeTableCat();*/
	return true;
}


bool catalogmanager::isExisttable(string tablenm) {
	vector<Table>::iterator ite;
	for (ite = this->tableList.begin(); ite != this->tableList.end(); ++ite) {
		if (ite->name == tablenm)
			return true;
	}
	return false;
}

bool catalogmanager::createIndex(string tablenm, string indexnm, string attrnm) {
	int type;
	if (isExistindex(indexnm, tablenm) == true) {
		return false;
	}
	vector<Table>::iterator ite;
	vector<Attribute>::iterator aite;
	for (ite = this->tableList.begin(); ite != this->tableList.end(); ++ite)
	{
		if (ite->name == tablenm)
		{
			aite = ite->attr.begin();
			while (aite->name != attrnm) {
				++aite;
			}
			if (aite == ite->attr.end())
				return false;


			aite->index = indexnm;//set hasIndex
			type = aite->type;
			Index node(tablenm, indexnm, attrnm);
			this->indexList.push_back(node);
			/*if (type == INT_TYPE) {
				BPlusTree<int> bt(tablenm, attrnm, indexnm);
				bt.createTree();
			}
			else if (type == FLOAT_TYPE) {
				BPlusTree<float> bt(tablenm, attrnm, indexnm);
				bt.createTree();
			}
			else if (type == CHAR_TYPE) {
				BPlusTree<string> bt(tablenm, attrnm, indexnm);
				bt.createTree();
			}
			*/
		}
	}return true;
}

bool catalogmanager::isExistindex(string indexname, string tablenm) {
	vector<Index>::iterator ite;
	for (ite = this->indexList.begin(); ite != this->indexList.end(); ++ite)
	{
		if (ite->indexName == indexname && ite->tbname == tablenm)
			return true;

	}

	return false;
}


bool catalogmanager::droptable(string tablenm) {
	vector<Table>::iterator ite;
	bool flag = false;
	string filename = tablenm + ".table";
	char * s = (char*)filename.data();
	remove(s);
	for (ite = this->tableList.begin(); ite != this->tableList.end();) {
		if (ite->name == tablenm) {
			ite = tableList.erase(ite);
			flag = true;
		}
		else {
			ite++;
		}
	}
	vector<Index>::iterator indexite;
	for (indexite = this->indexList.begin(); indexite != this->indexList.end();) {
		if (indexite->tbname == tablenm) {
			string index_name = indexite->indexName;
			string filename = index_name + ".index";
			char * s = (char*)filename.data();
			remove(s);
			indexite = this->indexList.erase(indexite);
		}
		else indexite++;
	}
	return flag;
}

bool catalogmanager::dropindex(string indexnm,string tablename) {
	vector<Index>::iterator ite;
	vector<Table>::iterator tite;
	string tablenm;
	bool flag = false;
	Table tb;
	int i;
	for (i = 0; i < tableList.size(); i++) {
		if (tablename == tableList[i].name) {
			tb = tableList[i];
			break;
		}
	}
	if (i == tableList.size()) {
		cout << "Error: Table '" + tablename + "' does not exist, check it " << endl;
		return false;
	}
	string filename = indexnm + ".index";
	char * s = (char*)filename.data();
	remove(s);
	for (ite = this->indexList.begin(); ite != this->indexList.end();) {
		
		if (ite->indexName == indexnm ) {
			tablenm = ite->tbname;
			ite = this->indexList.erase(ite);
			flag = true;
			break;
		}
		else ite++;
	}
	for (tite = this->tableList.begin(); tite != this->tableList.end(); tite++) {
		if (tite->name == tablenm) {
			vector<Attribute>::iterator aite;
			for (aite = tite->attr.begin(); aite != tite->attr.end(); aite++)
				if (aite->index == indexnm)
					aite->index = "empty";
		}
	}
	return true;
}

bool catalogmanager::alterRecord(string tablenm, int num) {
	vector<Table>::iterator ite;
	for (ite = this->tableList.begin(); ite != this->tableList.end(); ite++) {
		if (ite->name == tablenm)
			ite->recordnum += num;
	}
	return true;
}

bool catalogmanager::addAttribute(string tablenm, Attribute newAttr) {
	vector<Table>::iterator ite;
	for (ite = this->tableList.begin(); ite != this->tableList.end(); ite++) {
		if (ite->name == tablenm) {
			ite->attr.push_back(newAttr);
			if (newAttr.type == INT_TYPE) {
				ite->recordLength += 4;
			}
			else if (newAttr.type == FLOAT_TYPE) {
				ite->recordLength += 4;
			}
			else if (newAttr.type == CHAR_TYPE) {
				ite->recordLength += newAttr.length;
			}
		}
	}
	return true;
}

bool catalogmanager::deleteAttribute(string tablenm, string oldAttr) {
	vector<Table>::iterator ite;
	int type;
	for (ite = this->tableList.begin(); ite != this->tableList.end(); ite++) {

		if (ite->name == tablenm) {
			vector<Attribute>::iterator aite;
			for (aite = ite->attr.begin(); aite != ite->attr.end();) {
				if (aite->name == oldAttr) {
					type = aite->type;
					aite = ite->attr.erase(aite);
				}
			}
			if (type == INT_TYPE) {
				ite->recordLength -= 4;
			}
			else if (type == FLOAT_TYPE) {
				ite->recordLength -= 4;
			}
			else if (type == CHAR_TYPE) {
				ite->recordLength -= 4;
			}
		}
	}return true;
}







