#ifndef _INDEXMANAGE_H_
#define _INDEXMANAGE_H_
#include <vector>
using namespace std;

template< class T >
class IndexNode  //using LeafNode to create
{
protected:
	BufferNode record; //the contetns of record corresponding to the key
	                   //will be null if the IndexNode dose not exist
	T key;             //the value of the key
	IndexNode* next;   //point to the next IndexNode
					   //will be null if the IndexNode is the last one	
public:
	IndexNode();
	~IndexNode();
	
	bool isEmpty();       //judge whether the IndexNode exist
	IndexNode getNext();  //get the next IndexNode
	T getKey();           //get the key value
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
	bool checkKey( int target ); //check whether the key with the value of taget exist or not
	T getKey( int i );
	InteriorNode* getChildPointer( int i );
	InteriorNode* getTargetPointer( int target );
};

template< class T >
class LeafNode:public InteriorNode
{
private:
	vector<???> recordAddress;
	LeafNode* nextLeafNode;

public:
	virtual bool isEmpty();
	??? getRecord( int target );
	??? getRecordAddress( int target );
    LeafNode* getNextLeaf();
};

#endif







