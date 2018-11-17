#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#define MAX_BLOCK_NUM 80
#define MAX_FILE_NUM 50
#define MAX_FILE_NAME 40
#define BLOCK_SIZE 4096
using namespace std;
class BlockNode{
public:
	bool pin;
	int offset;
	char * content;
	bool dirty;
	string fileName;
	BlockNode * prev;
	BlockNode * next;
	BlockNode();
	~BlockNode();
	BlockNode(string fileName,bool pin,int offset);
	void flush();
	char*  readBlock(int length,int offset);
	void writeBlock(string data,int length,int offset);
	void writeBlock(char * data, int length, int offset);
	void setBlockContent();
};


class BufferManager{
private:
	
	bool isfull();	
public:
	int total_block;
	BlockNode * block_head;
	BlockNode * block_tail;
	BufferManager();
	~BufferManager() {
		flushAll();
		delete block_head;
		delete block_tail;
	};
	BlockNode * getblock(string fileName,int offset,bool pin);
	//string readBlock(block_node * block, int length, int offset);
	//void writeBlock(block_node* block,string data, int length, int offset);
	void flushAll();
	//void flush(block_node* block);	
};

#endif