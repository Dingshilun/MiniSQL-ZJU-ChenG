#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include<string>
#include<vector>
using namespace std;
#define CREATE 	1001
#define DROP	1002
#define SELECT	1003
#define INSERT	1004
#define DELETE	1005
#define TABLE	1051
#define INDEX	1052

union Union{
		char *s;
		int n;
		float f;
};

struct TreeNode{
	string var;
	int op; //0: =; 1: <>; 2: <; 3: >; 4: <=; 5: >=;
	Union value;
};

typedef struct Def{
	string var;
	int type; //1: int; 2: char; 3: float
	int len;
}Definition;

class Interface{
public:
	int whichOperation(){
		return operation;
	}
	int whichObject(){
		return object;
	}
	int getPrimaryKeyIndex(){
		return pkey;
	}
	Definition getPrimaryKey(){
		return definitions[pkey];
	}
	string indexOnWhichTable(){
		return tableName;
	}
	Definition indexOnWhichColumn(){
		return column;
	}
	string getIndexName(){
		return indexName;
	}
	string getTableName(){
		return tableName;
	}
	vector<TreeNode> getConditions(){
		return conditions;
	}
	TreeNode getConditionByIndex(int index){
		return conditions[index];
	}
	vector<Union> getData(){
		return data;
	}
	Union getDatumByIndex(int index){
		return data[index];
	}
private:
	int operation;	//create, drop, select, insert, delete
	int object;		//table, index
	string tableName;
	string indexName;
	vector<TreeNode> conditions; 	//delete from <tableName> where <conditions>
									//select * from <tableName> where <conditions>
	int pkey;
	vector<Definition> definitions; //create <object(table)> <definitions> primary key definitions[<pkey>]
	
	Definition column;	//create <object(index)> on <tableName>(<column>)
	vector<Union> data;	//insert <data> into <tableName> 
	//drop <object> <name>
};

#endif