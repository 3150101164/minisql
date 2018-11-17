#include "BufferManager.h"
#include "MiniSQL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

BlockNode::~BlockNode() {
	delete[] content;
}

BlockNode::BlockNode(string fileName,bool pin,int offset){
	this->pin = pin;
	this->content = new char[BLOCK_SIZE];
	this->dirty = false;
	this->fileName = fileName;
	this->offset = offset;
	this->prev = NULL;
	this->next = NULL;
}

BlockNode::BlockNode(){
	pin = false;
	content = new char[BLOCK_SIZE];
	dirty = false;
	//fileName = new char[MAX_FILE_NAME];
	prev = NULL;
	next = NULL;
};
void BlockNode::flush(){
    if(!this->dirty){
        return;
    }
    else{
		
		FILE *fp;
		char * s = (char*)fileName.data();
		fp = fopen(s, "rb+");
		if (fp)
		{
			if (fseek(fp, offset * BLOCK_SIZE, 0) == 0)
			{
				if (fwrite(content,BLOCK_SIZE, 1, fp) != 1)
				{
					cout << "File Writing Error" << endl;
					exit(6);
				}
			}
			else
			{
				cout << "File Seeking Error" << endl;
				exit(4);
			}
		}
		else
		{
			cout << "File Opening Error" << endl;
			exit(5);
		}
		fclose(fp);
    }
}
char*  BlockNode::readBlock(int length, int offset){
	char* c = new char[length];
	memcpy(c, this->content + offset, length);
	return c;
}

void BlockNode::writeBlock(char * data, int length, int offset) {
	this->dirty = true;
	memcpy(this->content + offset, data, length);
	//memset(this->content + offset + data.size(), 0, length - data.size());
}

void BlockNode::writeBlock(string data, int length, int offset){
	this->dirty = true;
	const char* c = data.c_str();
	memcpy(this->content + offset, c, data.size());
	memset(this->content + offset + data.size(), 0, length - data.size());
}

//初始化的时候用
void BlockNode::setBlockContent(){
	fstream file;
	file.open(this->fileName,ios::in | ios::out | ios::app |ios::binary);
	if(file.good()){
		file.seekg(ios::beg + this->offset * BLOCK_SIZE);
		file.read(this->content,BLOCK_SIZE);
		file.close();
	}else{
		cerr << "file open fail!"<<endl;
	}
}


void BufferManager::flushAll(){
    BlockNode *btmp = NULL;
   
    for(btmp = this->block_head->next ; btmp != NULL;btmp = btmp->next){
        btmp->flush();
    }
}
bool BufferManager::isfull(){
	return this->total_block >= MAX_BLOCK_NUM;
}


BlockNode * BufferManager::getblock(string fileName,int offset,bool pin = false)
{
	BlockNode * btmp;
	if(total_block == 0){
		btmp = new BlockNode(fileName,pin,offset);
		btmp->setBlockContent();
		this->block_head->next = btmp;
		btmp->prev = this->block_head;
		this->block_tail = btmp;
		//this->block_tail = btmp;
		this->total_block ++;
		return btmp;
	}
	else{
		btmp = this->block_head->next;
		while(btmp){
			//缓冲区中存在对应的块
			if(fileName == btmp->fileName && offset == btmp ->offset){
				if(btmp->next){
					btmp->next->prev = btmp->prev;
					btmp->prev->next = btmp->next;
				}
				else{
					this->block_tail = btmp->prev;
					if (block_tail == block_head) {
						block_tail = block_head->next;
					}
					btmp->prev->next = NULL;
				}
				btmp->next = this->block_head->next;
				if(btmp ->next)
					btmp->next->prev = btmp;
				this -> block_head->next = btmp;
				btmp -> prev = this->block_head;
				return btmp;
			}
			btmp = btmp ->next;
		}
		//没找到对应的块，需要替换或者添加块
		btmp = new BlockNode(fileName,pin,offset);
		btmp->setBlockContent();
		//需要替换
		if(isfull()){
			BlockNode * bb = this->block_tail;
			while(bb->pin)
				bb = bb->prev;
			bb->flush();
			if(bb == this->block_tail){
				//BlockNode * tt = this->block_tail;
				this->block_tail = this->block_tail->prev;
				block_tail->next = NULL;
				delete bb;
				
			}else{
				bb->prev->next = bb->next;
				bb->next->prev = bb->prev;
			}
			btmp ->next = this->block_head->next;
			this->block_head->next ->prev = btmp;
			btmp ->prev	= this->block_head;
			this->block_head ->next = btmp;
			return btmp;
			
		}
		//添加块
		else{
			total_block++;
			btmp->next = this->block_head->next;
			this->block_head->next->prev = btmp;
			btmp->prev = this->block_head;
			this->block_head->next = btmp;
			return btmp;
		}
	}
}



BufferManager::BufferManager(){
	total_block = 0;
	block_head = new BlockNode();
	block_tail = new BlockNode();
	//file_head = malloc(sizeof(file_node));
}


//int main(){
//	BufferManager * buffer = new BufferManager ();
//	BlockNode*  block = buffer->getblock("10000.txt",0,false);
//	cout << buffer->getblock("10000.txt",0,false)->content<<endl;
//	buffer->getblock("10000.txt",3,false);
//	cout <<buffer->getblock("10000.txt",1,false)<<endl;
//	BlockNode *tmp;
//	for (tmp = buffer->block_head; tmp !=NULL;tmp = tmp->next){
//		cout << tmp->content<<endl;
//	}
//	
//}





