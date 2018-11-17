#include "IndexManager.h"
#include "BufferManager.h"
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include <fstream>

using namespace std;

template<class T>
BPlusNode<T>  *  readBPlusNode(int block_off) {

}

template <class T>
vector<record>  BPlusTree<T>::findRecords(compare c, T key){
	vector <record> r;
	BPlusNode<T> * root = &Nodes[root_index];
	int leaf_index = root_index;
	int i;
	//find leaf node 
	while (1) {
		for (i = 0; i < root->nkey && root->key[i] <= key; i++) {}
		if (!root->isleaf) {
			leaf_index = root->pointer[i];
			root = &Nodes[root->pointer[i]];
		}
		else {
			break;
		}
	}
	if (c == bt) {
		int i;
		for (i = 0; i < root->nkey; i++) {
			if (root->key[i] > key)
				break;
		}
		while (1) {
			for (int j = i; j < root->nkey; j++) {
				r.push_back(root->record_pointer[j]);
			}
			if (root->pointer[MAX] == -1)
				break;
			root = &Nodes[root->pointer[MAX]];
			i = 0;
		}
		return r;
	}
	else if (c == be) {
		int i;
		for (i = 0; i < root->nkey; i++) {
			if (root->key[i] >= key)
				break;
		}
		while (1) {
			for (int j = i; j < root->nkey; j++) {
				r.push_back(root->record_pointer[j]);
			}
			if (root->pointer[MAX] == -1)
				break;
			root = &Nodes[root->pointer[MAX]];
			i = 0;
		}
		return r;
	}
	else if (c == se) {
		int i;
		for (i = 0; i < root->nkey; i++) {
			if (root->key[i] > key)
				break;
		}
		i--;
		int index = data_index;
		BPlusNode<T> * btmp = &Nodes[data_index];
		while (1) {
			if (index == leaf_index) {
				for (int j = 0; j <= i; j++) {
					r.push_back(root->record_pointer[j]);
				}
				break;
			}
			else {
				for (int j = 0; j < btmp->nkey; j++) {
					r.push_back(btmp->record_pointer[j]);
				}
				if (btmp->pointer[MAX] == -1)
					break;
				index = btmp->pointer[MAX];
				btmp = &Nodes[btmp->pointer[MAX]];
			}
		}
		return r;
	}

	else if (c == st) {
		int i;
		for (i = 0; i < root->nkey; i++) {
			if (root->key[i] >= key)
				break;
		}
		i--;
		int index = data_index;

		BPlusNode<T> * btmp = &Nodes[data_index];
		while (1) {

			if (index == leaf_index) {
				for (int j = 0; j <= i; j++) {
					r.push_back(root->record_pointer[j]);
				}
				break;
			}
			else {
				for (int j = 0; j < btmp->nkey; j++) {
					r.push_back(btmp->record_pointer[j]);
				}
				if (btmp->pointer[MAX] == -1)
					break;
				index = btmp->pointer[MAX];
				btmp = &Nodes[btmp->pointer[MAX]];
				
			}
		}
		return r;
	}
	else if (c == eq) {
		for (int j = 0; j < root->nkey; j++) {
			if (root->key[j] == key) {
				r.push_back(root->record_pointer[j]);
			}
		}
		return r;
	}
	else if (c == neq) {
		bool flag = false;
		int j;
		for (j = 0; j < root->nkey; j++) {
			if (root->key[j] == key) {
				flag = true;
				break;
			}
		}
		if (!flag) {
			BPlusNode<T> * btmp = &Nodes[data_index];
			while (1) {
				for (int i = 0; i < btmp->nkey; i++) {
					r.push_back(btmp->record_pointer[i]);
				}
				if (btmp->pointer[MAX] == -1)
					break;
				btmp = &Nodes[btmp->pointer[MAX]];
			}
		}
		else {
			int index = data_index;
			BPlusNode<T> * btmp = &Nodes[index];
			while (1) {
				if (index == leaf_index) {
					for (int i = 0; i < btmp->nkey; i++) {
						if (i != j)
							r.push_back(btmp->record_pointer[i]);
					}
					if (btmp->pointer[MAX] == -1)
						break;
					btmp = &Nodes[btmp->pointer[MAX]];
					index = btmp->pointer[MAX];
				}
				else {
					for (int i = 0; i < btmp->nkey; i++) {
						r.push_back(btmp->record_pointer[i]);
					}
					if (btmp->pointer[MAX] == -1)
						break;
					index = btmp->pointer[MAX];
					btmp = &Nodes[btmp->pointer[MAX]];
					
				}
			}
		}
	}
}


template <class T>
void BPlusTree<T>::createTree() {
	BPlusNode<T> node(true,true);
	node.isroot = 1;
	root_index = 0;
	node.isleaf = 1;
	nNode++;
	Nodes[0] = node;
	//&Nodes[0] = new BPlusNode<T>(true,true);
	ofstream fs;
	//创建index文件
	string s = indexName + ".index";

	fs.open(s, ios::binary);
	fs.close();
	writeTree();
}
template <class T>
record * BPlusTree<T>::searchKey(T key) {
	BPlusNode<T>*  root;
	root = &Nodes[root_index];
	while (1) {
		int i = 0;
		for (i = 0; i < root->nkey && root->key[i] <= key; i++) {}
		if(root->isleaf) {
			if (root->key[i - 1] == key)
				return &root->record_pointer[i];
			else{
				//cout << "can't find " << endl;
				return NULL;
			}
		}
		root = &Nodes[root->pointer[i]];
	}
}
template <class T>
void BPlusTree<T>::writeTree() {
	int i;
	ofstream file;
	string s = indexName + ".index";
	file.open(s, ios::binary);
	//缓存的类型是 unsigned char *，需要类型转换
	file.write((char *)this, sizeof(BPlusTree));  //winServer为类对象  
	/*cout << this->nNode;
	for (i = 0; i < this->nNode; i++) {
		file.write((char * )&this->Nodes[i],sizeof(BPlusNode));
	}*/
	file.close();
}

template <class T>
void BPlusTree<T>::insertKey(T key) {
	if (searchKey(key)) {
		cout << "The key is already in B Plus Tree" << endl;
		return;
	}
	BPlusNode<T> * root = &Nodes[root_index];
	int leaf_index = root_index;
	int i;
	while (1) {
		for (i = 0; i < root->nkey && root->key[i] <= key; i++) {}
		if (!root->isleaf) {
			leaf_index = root->pointer[i];
			root = &Nodes[root->pointer[i]];
		}
		else {
			break;
		}
	}
	//split
	if (root->nkey == MAX) {
		splitBplusNode(leaf_index, 0);
		//writeBplusNode(root, root);
		//writeBplusNode(root, newRoot);
		root = &Nodes[root_index];
		while (1) {
			for (i = 0; i < root->nkey && root->key[i] <= key; i++) {}
			if (!root->isleaf) {
				leaf_index = root->pointer[i];
				root = &Nodes[root->pointer[i]];
			}
			else
				break;
		}
	}
	if (root->nkey == 0) {
		root->key[0] = key;
		root->nkey++;
	}
	else {
		for (int k = root->nkey; k > i; k--) {
			root->key[k] = root->key[k - 1];
			root->pointer[k + 1] = root->pointer[k];
		}
		root->pointer[i + 1] = root->pointer[i];
		//root.pointer[i] = ? ? ;
		root->key[i] = key;
		root->nkey++;
	}
}
template <class T>
void BPlusTree<T>::insertKey(T key,record  r) {
	if (searchKey(key)) {
		return;
	}
	BPlusNode<T> * root = &Nodes[root_index];
	int leaf_index = root_index;
	int i;
	//find leaf node
	while (1) {
		for (i = 0; i < root->nkey && root->key[i] < key; i++) {}
		if (!root->isleaf) {
			leaf_index = root->pointer[i];
			root = &Nodes[root->pointer[i]];
		}
		else {
			break;
		}
	}

	//split
	if (root->nkey == MAX) {
		splitBplusNode(leaf_index, 0);
		//writeBplusNode(root, root);
		//writeBplusNode(root, newRoot);
		root = &Nodes[root_index];
		while (1) {
			for (i = 0; i < root->nkey && root->key[i] < key; i++) {}
			if (!root->isleaf) {
				leaf_index = root->pointer[i];
				root = &Nodes[root->pointer[i]];
			}
			else
				break;
		}
	}
	//root : leaf node
	if (root->nkey == 0) {
		root->key[0] = key;
		root->record_pointer[0] = r;
		root->nkey++;
	}
	else {
		for (int k = root->nkey; k > i; k--) {
			root->key[k] = root->key[k - 1];
			root->pointer[k + 1] = root->pointer[k];
			root->record_pointer[k] = root->record_pointer[k - 1];
		}
		root->pointer[i + 1] = root->pointer[i];
		//root.pointer[i] = ? ? ;
		root->key[i] = key;
		root->record_pointer[i] = r;
		root->nkey++;
	}
}

template <class T>
void BPlusTree<T>::splitBplusNode(int current_index, const int childnum){
	int half = ceil(MAX * 1.0 / 2);
	int i;
	BPlusNode<T>& current = Nodes[current_index];
	if (current.isroot) {
		
		int  father_index = newBplusNode();
		BPlusNode<T>& father =Nodes[father_index];
		father.isroot = 1;
		father.pointer[0] = current_index;
		root_index = father_index;
		current.father = father_index;
		current.isroot = 0;
	}
	BPlusNode<T>& father = Nodes[current.father];
	T key = current.key[half];
	int k;
	for (k = 0; k < father.nkey && father.key[k] < key; k++) {}
	for (i = father.nkey; i > k; i--)
	{
		father.key[i] = father.key[i - 1];
		father.pointer[i + 1] = father.pointer[i];
	}
	father.nkey++;
	BPlusNode<T> t;
	t.father = current.father;
	int address = newBplusNode();
	father.key[k] = current.key[half];
	father.pointer[k + 1] = address;
	/*if (current.isleaf) {
		for (i = half; i < MAX; i++) {
			t.key[i - half] = current.key[i];
			t.record_pointer[i - half] = current.record_pointer[i];
		}
	}
	else {

	}*/
	for (i = half; i < MAX; i++)
	{
		t.key[i - half ] = current.key[i];
		t.pointer[i - half] = current.pointer[i];
		t.record_pointer[i - half] = current.record_pointer[i];
	}

	t.nkey = MAX - half;
	t.pointer[t.nkey] = current.pointer[MAX];

	t.isleaf = current.isleaf;
	t.father = current.father;
	current.nkey = half;

	if (current.isleaf)   //如果当前被分裂节点是叶子
	{
		t.pointer[MAX] = current.pointer[MAX];
		current.pointer[MAX] = address;
	}
	else {
		deletekeyfromInterNode(t.key[0], &t);
		for (int i = 0; i <= t.nkey; i++) {
			Nodes[t.pointer[i]].father = address;
		}
	}
	Nodes[address] = t;
	//writeBplusNode(address, t);
	if (father.nkey == MAX)
		splitBplusNode(current.father,0);
}

template <class T>
int BPlusTree<T>::newBplusNode(){
	BPlusNode<T> node;
	Nodes[nNode++] = node;
	return nNode-1;
}
template <class T>
void BPlusTree<T>::deleteKey(T key) {
	if (searchKey(key) == NULL)
		return;
	else {
		//首先找到对应key叶子节点
		int current_index;
		BPlusNode<T> * root = &Nodes[root_index];
		int leaf_index = root_index;
		int prev_index;
		int i;
		while (1) {
			for (i = 0; i < root->nkey && root->key[i] <= key; i++) {}
			if (!root->isleaf) {
				prev_index = i;
				leaf_index = root->pointer[i];
				root = &Nodes[root->pointer[i]];
			}
			else {
				break;
			}
		}
		current_index = leaf_index;
		//在叶节点中删除对应的index
		deletekeyfromLeafNode(key, root);
		if (root->nkey >= (ceil(MAX * 1.0 / 2) - 1)) {   //大于半满删除结束
			return;
		}
		else {   //指针重新分配或者向兄弟借一个节点
			BPlusNode<T>* father_node = &Nodes[root->father];
			int index;
			//寻找左右兄弟
			for (index = 0; index <= father_node->nkey; index++) {
				if (father_node->pointer[index] == current_index)
					break;
			}
			BPlusNode<T> * left_sibling;
			BPlusNode<T> * right_sibling;
			if (index == 0) {
				left_sibling = NULL;
				right_sibling = &Nodes[father_node->pointer[index + 1]];
			}
			else if (index == father_node->nkey) {
				left_sibling = &Nodes[father_node->pointer[index - 1]];
				right_sibling = NULL;
			}
			else {
				left_sibling = &Nodes[father_node->pointer[index - 1]];
				right_sibling = &Nodes[father_node->pointer[index + 1]];
			}

			if (left_sibling != NULL && left_sibling->nkey > (ceil(MAX*1.0 / 2) - 1)) { //从左兄弟借一个（tested）
				T k_ = left_sibling->key[left_sibling->nkey - 1];
				Nodes[root->father].key[index - 1] = k_;
				for (int m = root->nkey; m >= 0; m--) {
					if (m == 0) {
						root->key[m] = left_sibling->key[left_sibling->nkey - 1];
						root->record_pointer[m] = left_sibling->record_pointer[left_sibling->nkey - 1];
						//root->pointer[m] = left_sibling->pointer[m - 1];
						left_sibling->nkey--;
					}
					else {  //m ！=0
						root->key[m] = root->key[m - 1];
						root->record_pointer[m] = root->record_pointer[m - 1];
					}
				}
				root->nkey++;
			}
			else if (right_sibling != NULL && right_sibling->nkey > (ceil(MAX *1.0 / 2) - 1)) { //从右兄弟那儿借一个（tested）
				T k_ = right_sibling->key[0];
				root->key[root->nkey] = k_;
				root->record_pointer[root->nkey] = right_sibling->record_pointer[0];
				deletekeyfromLeafNode(k_, right_sibling);
				Nodes[root->father].key[index] = right_sibling->key[0];
				root->nkey++;
			}

			else {   //需要进行合并
				if (left_sibling != NULL) {  //tested
					//合并
					for (int j = left_sibling->nkey; j < left_sibling->nkey + root->nkey; j++) {
						left_sibling->key[j] = root->key[j - left_sibling->nkey];
						left_sibling->record_pointer[j] = root->record_pointer[j - left_sibling->nkey];
					}
					left_sibling->pointer[MAX] = root->pointer[MAX];
					Nodes[current_index] = *left_sibling;
					//delete father key 
					T k_ = Nodes[root->father].key[index - 1];
					deletekeyfromInterNode(k_, &Nodes[root->father]);
					current_index = root->father;
				}
				else if (right_sibling != NULL) {    // not tested
					current_index = root->father;
					//join
					for (int j = root->nkey; j < root->nkey + right_sibling->nkey; j++) {
						root->key[j] = right_sibling->key[j - root->nkey];
						root->record_pointer[j] = right_sibling->record_pointer[j - root->nkey];
					}
					root->nkey += right_sibling->nkey;
					root->pointer[MAX] = right_sibling->pointer[MAX];
					T k_ = Nodes[root->father].key[index];
					deletekeyfromInterNode(k_, &Nodes[root->father]);
					Nodes[root->father].pointer[index] = leaf_index;
				}
				BPlusNode<T>* btmp = &Nodes[current_index];
				if (btmp->nkey >= (ceil(MAX * 1.0 / 2) - 1))  //删除结束
					return;
				else {
					while (1) {
						BPlusNode<T>* btmp = &Nodes[current_index];
						BPlusNode<T>* father_node = &Nodes[btmp->father];
						for (index = 0; index < father_node->nkey; index++) {
							if (father_node->pointer[index] == current_index)
								break;
						}
						if (btmp->isroot) {
							if (btmp->nkey <= 0)
							{
								root_index = btmp->pointer[0];
								return;
							}
						}
						//找到当前节点的左右兄弟
						if (index == 0) {
							left_sibling = NULL;
							right_sibling = &Nodes[father_node->pointer[index + 1]];
						}
						else if (index == father_node->nkey) {
							left_sibling = &Nodes[father_node->pointer[index - 1]];
							right_sibling = NULL;
						}
						else {
							left_sibling = &Nodes[father_node->pointer[index - 1]];
							right_sibling = &Nodes[father_node->pointer[index + 1]];
						}

						if (left_sibling != NULL && left_sibling->nkey > ceil(MAX*1.0 / 2) - 1) { //兄弟节点有富余  tested
							T k_ = father_node->key[index - 1];
							T curr_key = btmp->key[0];
							T left_key = left_sibling->key[left_sibling->nkey - 1];
							father_node->key[index - 1] = left_key;
							btmp->pointer[btmp->nkey + 1] = btmp->pointer[btmp->nkey];
							for (int m = btmp->nkey; m >= 0; m--) {
								if (m == 0)
									btmp->key[m] = k_;
								else {
									btmp->key[m] = btmp->key[m - 1];
									btmp->pointer[m] = btmp->pointer[m - 1];
								}
							}
							btmp->nkey++;
							btmp->pointer[0] = left_sibling->pointer[left_sibling->nkey];
							left_sibling->nkey--;
							return;
						}
						else if (right_sibling != NULL && right_sibling->nkey > ceil(MAX*1.0 / 2) - 1) { //兄弟节点有富余  tested
							T k_ = father_node->key[index];
							btmp->key[btmp->nkey] = k_;
							btmp->nkey++;
							btmp->pointer[btmp->nkey] = right_sibling->pointer[0];
							father_node->key[index] = right_sibling->key[0];
							deletekeyfromInterNode(right_sibling->key[0], right_sibling);
							return;
						}
						else {  //需要合并
							if (left_sibling != NULL) {    //not tested
								int tmp = father_node->pointer[0];
								left_sibling->key[left_sibling->nkey] = father_node->key[index - 1];
								deletekeyfromInterNode(father_node->key[index - 1], father_node);
								left_sibling->nkey++;
								for (int j = 0; j < btmp->nkey; j++) {
									left_sibling->key[left_sibling->nkey + j] = btmp->key[j];
									left_sibling->pointer[left_sibling->nkey + j] = btmp->pointer[j];
								}
								left_sibling->nkey += btmp->nkey;
								left_sibling->pointer[left_sibling->nkey] = btmp->pointer[btmp->nkey];
								if (father_node->isroot && father_node->nkey == 0) {
									root_index = tmp;
									return;
								}
								else if (father_node->isroot) {
									return;
								}
							}
							else {  //tested
								btmp->key[btmp->nkey] = father_node->key[index];
								deletekeyfromInterNode(father_node->key[index], father_node);
								btmp->nkey++;
								for (int i = 0; i < right_sibling->nkey; i++) {
									btmp->key[i + btmp->nkey] = right_sibling->key[i];
									btmp->pointer[i + btmp->nkey] = right_sibling->pointer[i];
								}
								btmp->nkey += right_sibling->nkey;
								btmp->pointer[btmp->nkey] = right_sibling->pointer[right_sibling->nkey];
								if (father_node->isroot && father_node->nkey == 0) {
									root_index = current_index;
									return;
								}
								else if (father_node->isroot)
									return;
							}
							current_index = btmp->father;
						}

					}


				}
			}
		}
	}
}
template <class T>
void BPlusTree<T>::deletekeyfromLeafNode(T key, BPlusNode<T>* btmp) {
	int i;
	//BlusNode * btmp = &Nodes[index];
	for (i = 0; i < btmp->nkey; i++) {
		if (btmp->key[i] == key)
			break;
	}
	for (int j = i; j < btmp->nkey - 1; j++) {
		btmp->key[j] = btmp->key[j + 1];
		btmp->record_pointer[j] = btmp->record_pointer[j + 1];
	}
	//btmp->pointer[btmp->nkey - 1] = btmp->pointer[btmp->nkey];
	btmp->nkey--;
}

template <class T>
void BPlusTree<T>::deletekeyfromInterNode(T key, BPlusNode<T> * btmp) {
	int index;
	for (index = 0; index < btmp->nkey; index++) {
		if (btmp->key[index] == key)
			break;
	}
	for (int j = index; j < btmp->nkey - 1; j++) {
		btmp->pointer[j] = btmp->pointer[j + 1];
		btmp->key[j] = btmp->key[j + 1];
	}
	btmp->pointer[btmp->nkey-1] = btmp->pointer[btmp->nkey];
	btmp->nkey--;
}

template <class T>
void BPlusTree<T>:: readTree() {
	ifstream file;
	string s = indexName + ".index";
	file.open(s, ios::binary);
	file.read((char*)this, sizeof(BPlusTree));
	file.close();
}

/*
int main() {
	BPlusTree<int> bt("niubi","student");
	bt.createTree();
	//bt.writeTreeNode(0);
	bt.insertKey(1);
	bt.insertKey(2);
	bt.insertKey(3);
	bt.insertKey(4);
	bt.insertKey(5);
	bt.insertKey(6);
	bt.insertKey(7);
	bt.insertKey(8);
	bt.insertKey(9);
	bt.insertKey(10);
	//cout << sizeof(BPlusTree);
	//bt.writeTree();
	//bt.insertKey(11);
	//bt.insertKey(12);
	//BPlusTree<int> bt;
	//bt.readTree("student");
	bt.deleteKey(4);
	bt.deleteKey(3);
	bt.deleteKey(2);
	bt.deleteKey(1);
	bt.writeTree();
	BPlusTree <int> b;
	b.readTree("student");
	//bt.deleteKey(5);
	//bt.deleteKey(9);
	//bt.deleteKey(10);
	//bt.deleteKey(8);
	system("pause");
}
*/