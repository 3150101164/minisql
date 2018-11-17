#include<iostream>
#include<string>
#include<vector>
#include<cstring>
#include"recordmanager.h"
#include"minisql.h"
#include"BufferManager.h"
#include<fstream>
#include <math.h>
#include"IndexManager.h"
#include <cstring>

using namespace std;
//extern BufferManager ob;

void EraseSpace(string &s)
{
	//ch可换成其他字符  
	const char ch = ' ';
	s.erase(s.find_last_not_of(" ") + 1);
	s.erase(0, s.find_first_not_of(" "));
}

 bool record_management::compare_function(Table table, vector<comparision_condition> conditions,turple &row)
{
	 bool flag = true;
	 for (int j = 0; j < conditions.size(); j++)//每一个比较条件
	 {
		 string value = conditions[j].value;
		 int cols = conditions[j].columnnum;//第几个属性
		 int temp = atoi(row.row_value[cols].c_str());//如果是整形用这个
		 float temp1 = atof(row.row_value[cols].c_str());//如果是float型用这个，char型直接比较应该可以吧。
		 switch (table.attr[cols].type)
		 {
		 case INT_TYPE:
			 switch (conditions[j].condition)
			 {

			 case bt:
				 if (!(temp > atoi(value.c_str())))
					 flag =  false;
				 break;
				                     //为什么不加这个return 永远返回false
			 case be:
				 if (!(temp >= atoi(value.c_str())))
					 flag = false;
				 break;
				
			 case st:
				 if (!(temp < atoi(value.c_str())))
					 flag =  false;
				 break;
				
			 case se:
				 if (!(temp <= atoi(value.c_str())))
					 flag = false;
				 break;
			 case eq:
				 if (temp != atoi(value.c_str()))
					 flag = false;
				 break;
			 case neq:
				 if (!(temp != atoi(value.c_str())))
					 flag =  false;
				 break;
			 }
			 break;
		 case CHAR_TYPE:
			 EraseSpace(row.row_value[cols]);
			 EraseSpace(value);
			 switch (conditions[j].condition)
			 {
			 case bt:
				 if (!(row.row_value[cols] > value))
					 flag = false;
				 break;
			 case be:
				 if (!(row.row_value[cols] >= value))
					 flag = false;
				 break;
			 case st:
				 if (!(row.row_value[cols]< value))
					 flag = false;
				 break;
			 case se:
				 if (!(row.row_value[cols] <= value))
					 flag = false;
				 break;
			 case eq:
				 if (!(row.row_value[cols] == value))
					 flag = false;
				 break;
			 case neq:
				 if (!(row.row_value[cols] != value))
					 flag = false;
				 break;
			 }
			 break;

		 case FLOAT_TYPE:
			 switch (conditions[j].condition)
			 {
			 case bt:
				 if (!(temp1 > atof(value.c_str())))
					 flag = false;
				 break;
			 case be:
				 if (!(temp1 >= atof(value.c_str())))
					 flag = false;
				 break;
			 case st:
				 if (!(temp1 < atof(value.c_str())))
					 flag = false;
				 break;
			 case se:
				 if (!(temp1 <= atof(value.c_str())))
					 flag = false;
				 break;
			 case eq:
				 if (!(temp1 == atof(value.c_str())))
					 flag = false;
				 break;
			 case neq:
				 if (!(temp1 != atof(value.c_str())))
					 flag = false;
				 break;
			 }
			 break;
		 }

	 }
	 return flag;
}

 turple record_management::spilt_turple(Table table, string row)
 {
	 turple out;
	 //int start = 0, finish = 0;
	 //for (int attr_count = 0; attr_count < table.attr.size(); attr_count++)
	 //{
		// start = finish;
		// if(table.attr[attr_count].type==CHAR_TYPE)
		//	 finish = start + table.attr[attr_count].length;
		// else
		// string col;
		// for (int i = start; i < finish; i++)
		// {
		//	 col += row[i];
		// }
		// out.row_value.push_back(col);
	 //}
	 string tmp;
	 int i = 0;
	 string empty = "%";
	 for (int j = 0; j < table.attr.size(); j++)
	 {
		 while (row[i] != empty[0]&& i<row.size())
		 {
			 tmp += row[i];
			 i += 1;
		 }
		 out.row_value.push_back(tmp);
	 }
	
	 return out;
 }

 char*  record_management::join_turple(Table table, turple &a)              //turple 变成string
 {
	 char * out = new char[table.recordLength];
	 char * t = out;
	 //string empty = "%";
	 for (int attr_count = 0; attr_count < table.attr.size(); attr_count++)
	 {
		 if (table.attr[attr_count].type == CHAR_TYPE)
		 {
			 char * tt = (char*)a.row_value[attr_count].data();
			 if (strlen(tt) <= table.attr[attr_count].length) {
				 memcpy(t, tt, strlen(tt));
				 t = t + strlen(tt);
				 memset(t, 32, table.attr[attr_count].length - strlen(tt));
				 t = t + table.attr[attr_count].length - strlen(tt) ;
				 
			 }
			 else {
				 memcpy(t, tt, table.attr[attr_count].length);
				 t = t + table.attr[attr_count].length;
			 }
			// t = t + strlen(tt);
		 }//char type 就补下空格再加分隔符
		 else  if(table.attr[attr_count].type == INT_TYPE)
		 {
			 int  tt = stoi(a.row_value[attr_count].c_str());
			 memcpy(t, &tt, sizeof(tt));
			 t = t + sizeof(tt);
		 }
		 else {
			 float tt = stof(a.row_value[attr_count].c_str());
			 memcpy(t, &tt, sizeof(tt));
			 t = t + sizeof(tt);
		 }
	 }
	 return out;
 }

 turple record_management::char2turple(Table table, char*s) {
	 vector<Attribute> attrs = table.attr;
	 turple out;
	 char * t = s;
	 char * tmp;
	 for (int i = 0; i < attrs.size(); i++) {
		if (attrs[i].type == INT_TYPE) {
			int x;
			memcpy(&x, t, 4);
			t = t + 4;
			string s = to_string(x);
			out.row_value.push_back(s);
		}
		else if (attrs[i].type == FLOAT_TYPE) {
			float x;
			memcpy(&x, t, 4);
			t = t + 4;
			string s = to_string(x);
			out.row_value.push_back(s);
		}
		else {
			int length = attrs[i].length;
			char * x = new char [length + 1];
			memcpy(x, t, length);
			x[length] = '\0';
			string niubi(x);
			t = t + length;
			out.row_value.push_back(niubi);
		}
	 }
	 return	out;
 }


 turples record_management::selection(Table &table, vector<comparision_condition> &conditions, vector<comparision_condition> &conditions_index, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string)
 { 
	 turple row;
	 turples output;
	 string filename = table.name + ".table";
	 int max_num = floor(BLOCK_SIZE *1.0 / table.recordLength);
	 int block_num = ceil(table.recordnum  *1.0 / max_num);
	 char * temp;
	 vector<record> final;
	 //如果全都没有index
	 if (conditions_index.size() == 0)
	 { 
		 int count = 0;
		 for (int block_offset = 0 ; block_offset < block_num; block_offset++)
		 {
			 BlockNode* a = ob.getblock(filename, block_offset,false);
			 for (int offset = 0; offset < max_num;offset ++)
			 {
				 bool flag = false;
				 int off = offset * table.recordLength;
				 temp = a->readBlock(table.recordLength, off);
				 for (int i = 0; i < table.recordLength; i++) {
					 if (temp[i] != 32) {
						 flag = true;
						 break;
					 }
				 }
				 if (!flag)
					 continue;
				 row = char2turple(table,temp);
				 if(compare_function(table,conditions,row)){
					 output.values.push_back(row);
				 } 
				 count ++;
				 if(count >= table.recordnum)
					 break;
			 }
			 //a->flush();
			 if (count < table.recordnum && block_offset == block_num - 1) {
				 block_num++;
			 }
		 }
	 }
	 //有index的情况下先去index里面找
	 else
	 {
		 bool flag = true;
		 for (int i = 0; i < conditions_index.size(); i++)
		 {

			 if (table.attr[conditions_index[i].columnnum].type == INT_TYPE)
			 {
				 int key = atoi(conditions_index[i].value.c_str());
				 for (int m = 0; m < bt_int.size(); m++) {
					 if (table.name == bt_int[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_int[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_int[m]->indexName) {
						 vector<record> tmp = bt_int[m]->findRecords(conditions_index[i].condition, key);
						 if (final.size() == 0 && flag)
						 {
							 final = tmp;
							 flag = false;
						 }
						 else {
							 vector <record> tt;
							 for (int i = 0; i < final.size(); i++) {
								 for (int j = 0; j < tmp.size(); j++) {
									 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
										 tt.push_back(final[i]);
										 break;
									 }
								 }
							 }
							 final = tt;
						 }
						 break;
					 }
				 }
				 //BPlusTree<int>* bshu = new BPlusTree<int>(table.name,table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);



			 }
			 else if (table.attr[conditions_index[i].columnnum].type == FLOAT_TYPE)
			 {

				 // BPlusTree<float>* bshu= new BPlusTree<float>(table.name,table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);
				 float key = atof(conditions_index[i].value.c_str());
				 for (int m = 0; m < bt_float.size(); m++) {
					 if (table.name == bt_float[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_float[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_float[m]->indexName) {
						 vector<record> tmp = bt_float[m]->findRecords(conditions_index[i].condition, key);
						 if (final.size() == 0 && flag)
						 {
							 final = tmp;
							 flag = false;
						 }
						 else {
							 vector <record> tt;
							 for (int i = 0; i < final.size(); i++) {
								 for (int j = 0; j < tmp.size(); j++) {
									 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
										 tt.push_back(final[i]);
										 break;
									 }
								 }
							 }
							 final = tt;
						 }
						 break;
					 }
				 }

			 }
			 else
			 {
				 string key = conditions_index[i].value;
				 for (int m = 0; m < bt_string.size(); m++) {
					 if (table.name == bt_string[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_string[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_string[m]->indexName) {
						 vector<record> tmp = bt_string[m]->findRecords(conditions_index[i].condition, key);
						 if (final.size() == 0 && flag)
						 {
							 final = tmp;
							 flag = false;
						 }
						 else {
							 vector <record> tt;
							 for (int i = 0; i < final.size(); i++) {
								 for (int j = 0; j < tmp.size(); j++) {
									 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
										 tt.push_back(final[i]);
										 break;
									 }
								 }
							 }
							 final = tt;
						 }
						 break;
					 }
				 }
				 // BPlusTree<string>* bshu = new BPlusTree<string>(table.name,table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);

			 }
		 }
		 for (int j = 0; j < final.size(); j++)
		 {
			 BlockNode* block = ob.getblock(filename, final[j].block_offset, false);
			 char * temp1 = block->readBlock(table.recordLength, final[j].content_offset);
			 row = char2turple(table, temp1);
			 if (conditions.size() != 0)                //如果condition非空就用这每一条record去判断是不是符合，否则直接插入
			 {
				 if (compare_function(table, conditions, row))
					 output.values.push_back(row);
			 }
			 else
				 output.values.push_back(row);
		 }

	 }
	 return output;
}
 
 void record_management::selection(Table &table,vector<record> &rec,turples &output)
 {
	 turple row;
	 string filename = table.name + ".table";
	 char* temp;
	 int  count = 0;
	 int max_num = floor(BLOCK_SIZE / table.recordLength);
	 int block_num = ceil(table.recordnum * 1.0 / max_num);
	 for (int block_offset = 0; block_offset < block_num; block_offset++)
	 {
		 BlockNode* a = ob.getblock(filename, block_offset, false);
		 /*int min= table.recordLength*table.recordnum - block_offset*BLOCK_SIZE;
		 if (min > BLOCK_SIZE)
		 min = BLOCK_SIZE;*/
		 for (int i = 1; i <= max_num; i++)
		 {
			 bool flag = false;
			 int offset = (i - 1) * table.recordLength;
			 temp = a->readBlock(table.recordLength, offset);
			 for (int i = 0; i < table.recordLength; i++) {
				 if (temp[i] != 32) {
					 flag = true;
					 break;
				 }
			 }
			 if (!flag)
				 continue;
			 record r(table.name,block_offset,offset,table.recordLength);
			 rec.push_back(r);
			 row = char2turple(table, temp);
			 delete temp;
			 output.values.push_back(row);
			 count++;
			 if (count >= table.recordnum)
				 break;
		 }
		 //a->flush();
		 if (count < table.recordnum && block_offset == block_num - 1)
			 block_num++;
	 }
 }

 turples record_management::selection(Table &table)
 {
	 turple row;
	 turples output;
	 string filename = table.name + ".table";
	 char* temp;
	 int  count = 0;
	 int max_num = floor(BLOCK_SIZE / table.recordLength);
	 int block_num = ceil(table.recordnum * 1.0 /max_num);
	 for (int block_offset = 0; block_offset < block_num; block_offset++)
	 {
		 BlockNode* a = ob.getblock(filename, block_offset, false);
		 /*int min= table.recordLength*table.recordnum - block_offset*BLOCK_SIZE;
		 if (min > BLOCK_SIZE)
			 min = BLOCK_SIZE;*/
		 for (int i = 1; i <= max_num; i++)
		 {
			 bool flag = false;
			 int offset = (i - 1) * table.recordLength;
			 temp = a->readBlock(table.recordLength, offset);
			 for (int i = 0; i < table.recordLength; i++) {
				 if (temp[i] != 32) {
					 flag = true;
					 break;
				 }
			 }
			 if (!flag)
				 continue;
			 row = char2turple(table,temp);
			 output.values.push_back(row);
			 count++;
			 if (count >= table.recordnum)
				 break;
		 }
		// a->flush();
		 if (count < table.recordnum && block_offset == block_num - 1)
			 block_num++;
	 }
	 return output;
 }

 bool record_management::insertion(Table &table,turple &a,record &rec, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string)
 {
	 char* input;
	 input = join_turple(table, a);
	 string file_name = table.name + ".table";
	 int max_num = floor(BLOCK_SIZE / table.recordLength);
	 int block_offset = ceil((table.recordnum + 1) * 1.0 / max_num) - 1;
	 int offset = (table.recordnum - block_offset * max_num) * table.recordLength;
	 /*int empty_blocksize = BLOCK_SIZE - (int(BLOCK_SIZE / table.recordLength) * table.recordLength);
	 int block_offset = table.recordLength * table.recordnum / (BLOCK_SIZE-empty_blocksize);
	 int offset = table.recordLength*table.recordnum - block_offset * (BLOCK_SIZE-empty_blocksize);*/
	 vector<comparision_condition> cond_in;
	 vector<comparision_condition> empty;
	 vector<comparision_condition> cond_uin;
	 bool flag = true;
	 for (int i = 0; i < table.attr.size(); i++) 
	 {
		 if (table.attr[i].isUnique) {
			 comparision_condition cc(eq, a.row_value[i], i);
			 if (table.attr[i].index != "empty") 
			 {
				 
				 cond_in.push_back(cc);
				 turples check = selection(table, empty, cond_in, bt_int, bt_float, bt_string);
				 if (check.values.size() != 0)
					 flag = false;
				 cond_in.clear();
			 }
			 else
			 {
				 cond_uin.push_back(cc);
				 turples check = selection(table, cond_uin, empty, bt_int, bt_float, bt_string);
				 if (check.values.size() != 0)
					 flag = false;
				 cond_uin.clear();
			 }
		 }
	 }
	 
	 if (!flag) {
		 cout << "Error : insert error, check unique requriement" << endl;
		 return false;
	 }
	 record r(table.name, block_offset, offset, table.recordLength);
	 rec = r;
	 BlockNode* node = ob.getblock(file_name, block_offset, false);
	 node->writeBlock(input, table.recordLength, offset);
	 //node->flush();
	 table.recordnum += 1;
	 cout << "Query OK! 1 row affected." <<endl;
	 return true;
	 //node->dirty = true;
 }

 void record_management::deletion(Table &table)
 {
	 string file_name = table.name + ".table";
	 char * out = new char[table.recordLength];
	 memset(out, 32, table.recordLength);
	 int max_num = floor(BLOCK_SIZE* 1.0 / table.recordLength);
	 int block_num = ceil(table.recordnum * 1.0 / max_num);
	 int count = 0;
	 for (int block_offset = 0; block_offset <block_num; block_offset++)
	 {
		 BlockNode* a = ob.getblock(file_name, block_offset, false);
		 for (int i = 0; i < max_num; i++ )
		 {
			 int offset = i * table.recordLength;
			 a->writeBlock(out, table.recordLength, offset);
			 count++;
			 if (count >= table.recordnum)
				 break;
		 }
		 //a->flush();
		 if (count < table.recordnum && block_offset == block_num - 1)
			 block_num++;
	 } 
	 table.recordnum = 0;
 }

 void record_management::deletion(Table &table, vector<comparision_condition> conditions, vector<comparision_condition>conditions_index, vector<BPlusTree<int>*>&bt_int, vector<BPlusTree<float>*> &bt_float, vector<BPlusTree<string>*>&bt_string)
 {
	 turple row;
	 turples output;
	 int number = 0;
	 string filename = table.name + ".table";
	 int max_num = floor(BLOCK_SIZE *1.0 / table.recordLength);
	 int block_num = ceil(table.recordnum  *1.0 / max_num);
	 char * temp;
	 vector<record> final;
	 char * out = new char[table.recordLength];
	 memset(out, 32, table.recordLength);
	 //如果全都没有index
	 if (conditions_index.size() == 0)
	 {
		 int count = 0;
		 for (int block_offset = 0; block_offset < block_num; block_offset++)
		 {
			 BlockNode* a = ob.getblock(filename, block_offset, false);
			 for (int offset = 0; offset < max_num; offset++)
			 {
				 bool flag = false;
				 int off = offset * table.recordLength;
				 temp = a->readBlock(table.recordLength, off);
				 for (int i = 0; i < table.recordLength; i++) {
					 if (temp[i] != 32) {
						 flag = true;
						 break;
					 }

				 }
				 if (!flag) continue;
				 row = char2turple(table, temp);
				 if (compare_function(table, conditions, row)) {
					 a->writeBlock(out, table.recordLength, off);
					 number++;
				 }
				 count++;
				 if (count >= table.recordnum)
					 break;
			 }
			 //a->flush();
			 if (count < table.recordnum && block_offset == block_num - 1) {
				 block_num++;
			 }
		 }

	 }
	 //有index的情况下先去index里面找
	 else
	 {
		 bool flag = true;
		 for (int i = 0; i < conditions_index.size(); i++)
		 {

			 if (table.attr[conditions_index[i].columnnum].type == INT_TYPE)
			 {
				 //BPlusTree<int>* bshu = new BPlusTree<int>(table.name, table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);
				 int key = atoi(conditions_index[i].value.c_str());
				 if (table.attr[conditions_index[i].columnnum].type == INT_TYPE)
				 {
					 int key = atoi(conditions_index[i].value.c_str());
					 for (int m = 0; m < bt_int.size(); m++) {
						 if (table.name == bt_int[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_int[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_int[m]->indexName) {
							 vector<record> tmp = bt_int[m]->findRecords(conditions_index[i].condition, key);
							 if (final.size() == 0 && flag)
							 {
								 final = tmp;
								 flag = false;
							 }
							 else {
								 vector <record> tt;
								 for (int i = 0; i < final.size(); i++) {
									 for (int j = 0; j < tmp.size(); j++) {
										 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
											 tt.push_back(final[i]);
											 break;
										 }
									 }
								 }
								 final = tt;
							 }
							 break;
						 }
					 }
				 }
				 else if (table.attr[conditions_index[i].columnnum].type == FLOAT_TYPE)
				 {
					 //BPlusTree<float>*bshu = new BPlusTree<float>(table.name, table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);
					 float key = atof(conditions_index[i].value.c_str());
					 if (table.attr[conditions_index[i].columnnum].type == INT_TYPE)
					 {
						 for (int m = 0; m < bt_float.size(); m++) {
							 if (table.name == bt_float[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_float[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_float[m]->indexName) {
								 vector<record> tmp = bt_float[m]->findRecords(conditions_index[i].condition, key);
								 if (final.size() == 0 && flag)
								 {
									 final = tmp;
									 flag = false;
								 }
								 else {
									 vector <record> tt;
									 for (int i = 0; i < final.size(); i++) {
										 for (int j = 0; j < tmp.size(); j++) {
											 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
												 tt.push_back(final[i]);
												 break;
											 }
										 }
									 }
									 final = tt;
								 }
								 break;
							 }
						 }
					 }
				 }
				 else
				 {
					 //BPlusTree<string> *bshu = new BPlusTree<string>(table.name, table.attr[conditions_index[i].columnnum].name, table.attr[conditions_index[i].columnnum].index);
					 string key = conditions_index[i].value;

					 if (table.attr[conditions_index[i].columnnum].type == INT_TYPE)
					 {
						 for (int m = 0; m < bt_string.size(); m++) {
							 if (table.name == bt_string[m]->tableName && table.attr[conditions_index[i].columnnum].name == bt_string[m]->attrName &&table.attr[conditions_index[i].columnnum].index == bt_string[m]->indexName) {
								 vector<record> tmp = bt_string[m]->findRecords(conditions_index[i].condition, key);
								 if (final.size() == 0 && flag)
								 {
									 final = tmp;
									 flag = false;
								 }
								 else {
									 vector <record> tt;
									 for (int i = 0; i < final.size(); i++) {
										 for (int j = 0; j < tmp.size(); j++) {
											 if (final[i].tableName == tmp[j].tableName && final[i].block_offset == tmp[j].block_offset && final[i].content_offset == tmp[j].content_offset&&final[i].length == tmp[j].length) {
												 tt.push_back(final[i]);
												 break;
											 }
										 }
									 }
									 final = tt;
								 }
								 break;
							 }
						 }
					 }
				 }
				 for (int j = 0; j < final.size(); j++)
				 {
					 BlockNode* block = ob.getblock(filename, final[j].block_offset, false);
					 char * temp1 = block->readBlock(table.recordLength, final[j].content_offset);
					 row = char2turple(table, temp1);
					 if (conditions.size() != 0)                //如果condition非空就用这每一条record去判断是不是符合，否则直接插入
					 {
						 if (compare_function(table, conditions, row))
						 {
							 block->writeBlock(out, table.recordLength, final[j].content_offset);
							 number++;
							 //block->flush();
						 }
					 }
					 else
					 {
						 block->writeBlock(out, table.recordLength, final[j].content_offset);
						 number++;
						 //block->flush();
					 }

				 }

				 table.recordnum -= number;
			 }
		 }
	 }
}
 


 void record_management::create(Table &table)
 {
	 
	 string file_name = table.name + ".table";
	 FILE *file = fopen(file_name.c_str(), "r");
	 if (file)
		 cout << "The table has existed." << endl;
	 else 
	 {
		 fstream fout(file_name.c_str(),ios::out);
	     fout.close();
	 }
	 //在创建之前，判断是否已经存在该table
 }

 void record_management::drop(Table &table)
 {

	 string file_name = table.name + ".table";
	 if (remove(file_name.c_str()) != 0)
		 perror("drop table failed");
 }

 void record_management::display_turples(turples &a)
 {
	 for (int i = 0; i < a.values.size(); i++)
	 {
		 for (int j = 0; j < a.values[i].row_value.size();j++)
			 cout << a.values[i].row_value[j];
		 cout << endl;
	 }
 }


 /*
 Buffermanagemment 
 int main()
 {
	 Table student("student",0,0);
	 Attribute init;
	 init.name = "id";
	 init.isPrimary=false;
	 init.isUnique = false;
	 init.length = 12;
	 init.type = CHAR_TYPE;
     student.attr.push_back(init);
	 
	 init.name = "age";
	 init.isPrimary = false;
	 init.isUnique = false;
	 init.length = 4;
	 init.type = INT_TYPE;
	 student.attr.push_back(init);
	 
	 student.recordLength = 16;

	 record_management ob1;
	 ob1.create(student);
	 turple input;
	 
	 input.row_value.push_back("3150101164");
	 input.row_value.push_back("20");

	 ob1.insertion(student, input);

	 input.row_value.clear();
	 input.row_value.push_back("3150101169");
	 input.row_value.push_back("21");
	 ob1.insertion(student, input);

	

	 comparision_condition a;
	 a.condition = eq;
	 a.columnnum = 1;
	 a.value = "20";
	 vector<comparision_condition> b;
	 b.push_back(a);
	 turples output=ob1.selection(student,b);

	 ob1.display_turples(output);
	 output = ob1.selection(student);

	 ob1.display_turples(output);

	 ob1.deletion(student, b);
	 
	 output = ob1.selection(student);
	 ob1.display_turples(output);

	 ob1.drop(student);
	 
	 getchar();
	 return 0;
 }
 */ 