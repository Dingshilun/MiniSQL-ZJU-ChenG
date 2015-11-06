#ifndef _INDEXMANAGE_H_
#define _INDEXMANAGE_H_
#include <map>
#include <io.h>
#include <iostream>
#include <math.h>
#include <String>
#include <stdio.h>
#include <vector>
#include "BufferManager.h"
using namespace std;

#define SINGLE 0
#define LARGER 1
#define NOTLESS 2
#define NOTLARGER 3
#define LESS 4

#define CHAR_MAX  15
#define INT_MAX   509
#define FLOAT_MAX 509

typedef int ADDRESS;

struct BplusNode 
{ 
    int[INT_MAX] key;       //key value  
    int[INT_MAX+1] Pointer; //in interior node it ponit to the child node
    						//in leaf node, the low 12 bit is the offset number
    						//              the high 20 bit is the block number
    int  nkey;              //the number of existing key
    bool isleaf ;           //will be true if the node is leaf node, otherwise it will be false
        
};

class BplusTree
{
private:
	string indexFileName;	
	ADDRESS root;
	FILE* Bfile;
	FILE* configFile;
	BufferNode temp_bf;

public: 
	BplusTree( string, BufferNode&  );
	~BplusTree();
	void readBplusNode( ADDRESS, BplusNode& );
	void writeBplusNode( ADDRESS, BplusNode& );
	bool buildBplusTree( string, int, BufferNode& );  	
	void insertBplusTree( Target& );
	void insert( ADDRESS, Target& );  
	void splitBplusNode( BplusNode&, BPlusNode&, const int );  
	vector<index_info>& searchBplusTree( Target&, ADDRESS ) const; 
	void deleteBplusTree( Target& );
	void Delete( ADDRESS, Target& ); 
};

class IndexManager: public BufferManager
{
private:
	map< string, BplusTree > indexTable; //store the B+ tree list
public:
	
//	bool createIndex( Target );
	vector<index_info>& search( Target ); 
	int insert( Target );
	int delete( Target );
//	int deleteIndex( Target );
	int exist( Target );

};

template< class T >
class Target 
{
private:
	string IndexFileName; //without ".index" 
//	string indexName;
	T key;
	index_info index;
	int type; //operation type

public:
	Target( string );
	void setIndexFileName( string );
	void setKey( T );
	void setType( int );
	void setIndex_info( index_info );
	string getIndexFileName( );
	T getKey( );
	index_info getIndex_info();
	int getType();
};


#endif









