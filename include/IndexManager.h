#ifndef _INDEXMANAGE_H_
#define _INDEXMANAGE_H_
#include <vector>
#include <map>
using namespace std;

#define SEARCHSINGLE 0
#define

class IndexManager
{
private:
	map< string, BplusTree > indexTable; //store the B+ tree list
public:
	bool createIndex( Target t, string indexName );
	indexResult* searchSingle( Target t ); 
	bool insert( Target t );
	bool deleteKey( Target t );
	bool deleteIndex( Target t );
};

struct indexResult
{
	int blockNumber;
	int offset;
	indexResult* next;
};

struct indexNode
{
	string tableName;
	string attribute;
	string inexName;
};

template< class T >
class Target 
{
private:
	string table;
	string attribute;
	T beginKey;
	T endKey;
	bool isSingle;
public:
	Target( string t, string a )
	Target( string t, string a, T x );
	Target( string t, string a, T x, T y );
	~Target();
	void setTable( string name );
	void setAttribute( string name );
	void setBeginKey( T x );
	void setEndKey( T x );
	string getTable();
	strinng getAttribute();
	T getKey();
	T getBeginKey();
	T getEndKey();
	bool isSingle();
};

template< class T >
class BplusTree
{
private:
	int M;
	InteriorNode root;

public:
	BplusTree();
	~BplusTree();
	bool isEmpty();
	bool insert( T target, ??? ); //'???' should be the address of the record in the database file
	bool delete( T target, ??? );
	bool deleteIndex();
	IndexNode rangeSearch( T beginTarget, T endTarget );
	IndexNode singleSearch( T target ); 
};

template < class T >
class InteriorNode
{
private:
	int M;  
	vector<T> key;
	vector<InteriorNode*> pointer;

public:
	InteriorNode();
	~InteriorNode();

	virtual bool isEmpty();
	bool checkKey( T target ); //check whether the key with the value of taget exist or not
	T getKey( int i );
	InteriorNode* getChildPointer( int i );
	InteriorNode* getTargetPointer( T target );
};

template< class T >
class LeafNode:public InteriorNode
{
private:
	vector<int> blockNumber;
	vector<int> offset;
	LeafNode* nextLeafNode;

public:
	virtual bool isEmpty();
	??? getRecordAddress( T target );
    LeafNode* getNextLeaf();
};

#endif







