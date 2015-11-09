#ifndef _INDEXMANAGE_H_
#define _INDEXMANAGE_H_
//#include "stdafx.h"
#include <map>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <String>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <vector>
#include <typeinfo>
#include "BufferManager.h"
#include "MINISQL.h"

using namespace std;
#define F_OK 0
#define SINGLE 0
#define LARGER 1
#define NOTLESS 2
#define NOTLARGER 3
#define LESS 4
#define EQUAL 5

#define PRECISE 6

#define MAX_CHAR  15
#define MAX_INT   509
#define MAX_FLOAT 509

typedef int ADDRESS;

struct BplusNodeInt
{
	int key[MAX_INT];       //key value
	int Pointer[MAX_INT + 1]; //in interior node it ponit to the child node
	//in leaf node, the low 12 bit is the offset number
	//              the high 20 bit is the block number
	int  nkey;              //the number of existing key
	bool isleaf;           //will be true if the node is leaf node, otherwise it will be false  
};

struct BplusNodeFloat
{
	float key[MAX_FLOAT];       //key value
	int Pointer[MAX_FLOAT + 1]; //in interior node it ponit to the child node
	//in leaf node, the low 12 bit is the offset number
	//              the high 20 bit is the block number
	int  nkey;              //the number of existing key
	bool isleaf;           //will be true if the node is leaf node, otherwise it will be false  
};

struct BplusNodeChar
{
	char key[MAX_CHAR][255];       //key value
	int Pointer[MAX_CHAR + 1]; //in interior node it ponit to the child node
	//in leaf node, the low 12 bit is the offset number
	//              the high 20 bit is the block number
	int  nkey;              //the number of existing key
	bool isleaf;           //will be true if the node is leaf node, otherwise it will be false  
};

struct emptyNodeList
{
	int root;
	int isUsed[BLOCK_SIZE / sizeof(int)-1];
};


template<class T>
class Target
{
public:
	string indexFileName; //without ".index"
	T key;
	index_info index;
	int type; //operation type

public:
	Target(string t)                  { indexFileName = t; }
	void setIndexFileName(string t)   { indexFileName = t; }
	void setKey(T x)                  { key = x; }
	void setType(int x)                 { type = x; }
	void setIndex_info(index_info i)  { index = i; }
	string getIndexFileName()          { return indexFileName; }
	T getKey()                       { return key; }
	index_info getIndex_info()          { return index; }
	int getType()                       { return type; }
};

struct TargetInt
{
	string indexFileName;
	int key;
	index_info index;
	int type;
};

struct TargetFloat
{
	string indexFileName;
	float key;
	index_info index;
	int type;
};

struct TargetChar
{
	string indexFileName;
	string key;
	index_info index;
	int type;
};

class IndexManager : public bufferManager
{ 
private: //B+ tree data and operation
	map< string, ADDRESS > rootTable;
	string fileName;
	string indexFileName;
	string configFileName;
	ADDRESS root;

	int  createBplusTree(TargetInt); //create a new B+ tree, including creating the index file and config file
	int  createBplusTree(TargetFloat);
	int  createBplusTree(TargetChar);
	void emptyBplusTree();	        //empty the current B+ tree and save its information 
	int  switchBplusTree(string); //switch the current B+ tree to another

	void readBplusNode(ADDRESS, BplusNodeInt&);
	void readBplusNode(ADDRESS, BplusNodeFloat&);
	void readBplusNode(ADDRESS, BplusNodeChar&);
	void writeBplusNode(ADDRESS, BplusNodeInt&);
	void writeBplusNode(ADDRESS, BplusNodeFloat&);
	void writeBplusNode(ADDRESS, BplusNodeChar&);

	int  newBplusNode(); //get the available node with the smallest block number
	void setNodeEmpty(ADDRESS);

	void insertBplusTree(TargetInt&);
	void insertBplusTree(TargetFloat&);
	void insertBplusTree(TargetChar&);
	void insertKey(ADDRESS, TargetInt&);
	void insertKey(ADDRESS, TargetFloat&);
	void insertKey(ADDRESS, TargetChar&);

	void splitBplusNodeInt(BplusNodeInt&, BplusNodeInt&, const int);
	void splitBplusNodeFloat(BplusNodeFloat&, BplusNodeFloat&, const int);
	void splitBplusNodeChar(BplusNodeChar&, BplusNodeChar&, const int);

	vector<index_info> searchBplusTree(TargetInt&, ADDRESS&);
	vector<index_info> searchBplusTree(TargetFloat&, ADDRESS&);
	vector<index_info> searchBplusTree(TargetChar&, ADDRESS&);

	void deleteBplusTree(TargetInt&);
	void deleteKey(ADDRESS, TargetInt&);
	void deleteBplusTree(TargetFloat&);
	void deleteKey(ADDRESS, TargetFloat&);
	void deleteBplusTree(TargetChar&);
	void deleteKey(ADDRESS, TargetChar&);


	void enumLeafKey(TargetInt);
	void enumLeafKey(TargetFloat);
	void enumLeafKey(TargetChar);

	void assign(int& x, int y) { x = y; }
	void assign(int& x, float y) { x = y; }
	void assign(int& x, string y) { x = -1; }
	void assign(float& x, int y) { x = y; }
	void assign(float& x, float y) { x = y; }
	void assign(float& x, string y) { x = -1; }
	void assign(string& x, int y) { x = ""; }
	void assign(string& x, float y) { x = ""; }
	void assign(string& x, string y) { x = y; }

	string toString(char *);
	int  toOffset(int);
	// float floatRound( float );
public:

	IndexManager();
	virtual ~IndexManager();
	template<class T>
	int  createIndex(Target<T>);

	void showCurrentTree();

	template<class T>
	vector<index_info> search(Target<T>);

	template<class T>
	void insert(Target<T>);

	template<class T>
	void listALlLeaf(Target<T>);

	template<class T>
	void Delete(Target<T>);

	template<class T>
	void switchTree(Target<T>);

	template<class T>
	void drop( Target<T> t );
};


template<class T>
void IndexManager::drop( Target<T> t )
{
    if( fileName == t.getIndexFileName() )
    {
        root = -1;
        fileName = "";
        indexFileName = "";
        configFileName = "";
        rootTable.erase( fileName );
    }
    else if( rootTable.count( t.getIndexFileName() ) == 1 )
        rootTable.erase( t.getIndexFileName() );
}
template<class T>
int IndexManager::createIndex(Target<T> t)
{
	if (typeid(t.getKey()) == typeid(int))
	{
		TargetInt tt;
		// tt.key = t.getKey();
		assign(tt.key, t.key);
		tt.indexFileName = t.indexFileName;
		tt.type = t.type;
		tt.index = t.index;
		return createBplusTree(tt);
	}
	else if (typeid(t.getKey()) == typeid(float))
	{
		TargetFloat tt;
		// tt.key = t.getKey();
		assign(tt.key, t.key);
		tt.indexFileName = t.indexFileName;
		tt.type = t.type;
		tt.index = t.index;
		return createBplusTree(tt);
	}
	else if (typeid(t.getKey()) == typeid(string))
	{
		TargetChar tt;
		// tt.key = t.getKey();
		assign(tt.key, t.key);
		tt.indexFileName = t.indexFileName;
		tt.type = t.type;
		tt.index = t.index;
		return createBplusTree(tt);
	}
	else
		return -1;
}



template<class T>
void IndexManager::switchTree(Target<T> t)
{
	string name = t.getIndexFileName();
	switchBplusTree(name);
}

template<class T>
void IndexManager::insert(Target<T> t)
{
	string name = t.getIndexFileName();
	if (switchBplusTree(name) == 0)
	{
		if (typeid(T) == typeid(int))
		{
			TargetInt tt;
			// tt.key = t.getKey();
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			tt.type = t.type;
			tt.index = t.index;
			insertBplusTree(tt);
		}
		else if (typeid(T) == typeid(float))
		{
			TargetFloat tt;
			// tt.key = t.getKey();
			assign(tt.key, t.key);
			tt.indexFileName = t.key;
			tt.type = t.type;
			tt.index = t.index;
			insertBplusTree(tt);
		}
		else if (typeid(T) == typeid(string))
		{
			TargetChar tt;
			// tt.key = t.getKey();
			assign(tt.key, t.key);
			tt.indexFileName = t.key;
			tt.type = t.type;
			tt.index = t.index;
			insertBplusTree(tt);
		}
	}
	else
		cerr << "switch tree erro" << endl;
}

template<class T>
void IndexManager::listALlLeaf(Target<T> t)
{
	string name = t.getIndexFileName();

	if (switchBplusTree(name) == 0)
	{
		if (typeid(T) == typeid(int))
		{
			TargetInt tt;
			enumLeafKey(tt);
		}
		else if (typeid(T) == typeid(float))
		{
			TargetFloat tt;
			enumLeafKey(tt);
		}
		else if (typeid(T) == typeid(string))
		{
			TargetChar tt;
			enumLeafKey(tt);
		}
	}
	else
		cerr << "switch tree error" << endl;
}

template<class T>
vector<index_info> IndexManager::search(Target<T> t)
{
	string name = t.getIndexFileName();
	vector<index_info> result;

	if (switchBplusTree(name) == 0)
	{
		if (typeid(T) == typeid(int))
		{
			TargetInt tt;
			// tt.key = t.key;
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			tt.type = t.type;
			ADDRESS temp;
			result = searchBplusTree(tt, temp);
		}
		else if (typeid(T) == typeid(float))
		{
			TargetFloat tt;
			// tt.key = t.key;
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			tt.type = t.type;
			ADDRESS temp;
			result = searchBplusTree(tt, temp);
		}
		else if (typeid(T) == typeid(string))
		{
			TargetChar tt;
			// tt.key = t.key;
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			tt.type = t.type;
			ADDRESS temp;
			result = searchBplusTree(tt, temp);
		}
	}
	else
		cerr << "switch tree error" << endl;

	return result;
}

template<class T>
void IndexManager::Delete(Target<T> t)
{
	string name = t.getIndexFileName();

	if (switchBplusTree(name) == 0)
	{
		if (typeid(T) == typeid(int))
		{
			TargetInt tt;
			// tt.key = t.key;
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			deleteBplusTree(tt);
		}
		else if (typeid(T) == typeid(float))
		{
			TargetFloat tt;
			// tt.key = t.key;
			assign(tt.key, t.key);
			tt.indexFileName = t.indexFileName;
			deleteBplusTree(tt);
		}
		else if (typeid(T) == typeid(string))
		{
			TargetChar tt;
			// tt.key = t.key
			assign(tt.key, t.key);;
			tt.indexFileName = t.indexFileName;;
			deleteBplusTree(tt);
		}
	}
	else
		cerr << "switch tree error" << endl;
}


#endif













