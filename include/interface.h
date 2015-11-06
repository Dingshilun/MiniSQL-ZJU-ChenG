#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include "MINISQL.h"
#include <string>
#include <vector>
#include"attrNode.h"

using namespace std;
struct Union{
	bool isNull = false;
	string s = "";
	int n = 0;
	float f = 0.0;
};

struct TreeNode{
	int id = -1;
	int type = 0;
	int op = 0; //0: =; 1: <>; 2: <; 3: >; 4: <=; 5: >=;
	Union value;
};

struct Definition{
	string var = "";
	int type = 0; //1: int; 2: char; 3: float
	int len = 0;
};

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
	attrNode getPrimaryKey(){
		return definitions[pkey];
	}
	attrNode getAttrNode(int i){
		return definitions[i];
	}
	vector<attrNode> getDefinitions(){
		return definitions;
	}
	string indexOnWhichTable(){
		return tableName;
	}
	int indexOnWhichColumn(){
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
	void setOperation(int i){
		operation = i;
	}
	void setObject(int i){
		object = i;
	}
	void setTableName(string name){
		tableName = name;
	}
	void setIndexName(string name){
		indexName = name;
	}
	void addCondition(TreeNode t){
		conditions.push_back(t);
	}
	void addData(Union u){
		data.push_back(u);
	}
	void setColumn(int i){
		column = i;
	}
	void addDefinition(attrNode def){
		definitions.push_back(def);
	}
	int findIndexOfDef(string name){
		for (int i = 0; i < definitions.size(); ++i)
		if (definitions[i].attrName == name){
			definitions[i].isPrimary = true;
			definitions[i].isUnique = true;
			return i;
		}
		return -1;
	}
	void setPrimaryKeyIndex(int index){
		pkey = index;
	}
private:
	int operation;	//create, drop, select, insert, delete
	int object;		//table, index
	string tableName;
	string indexName;
	vector<TreeNode> conditions; 	//delete from <tableName> where <conditions>
	//select * from <tableName> where <conditions>
	int pkey;
	vector<attrNode> definitions; //create <object(table)> <definitions> primary key definitions[<pkey>]
	int column;	//create <object(index)> on <tableName>(<column>)
	vector<Union> data;	//insert <data> into <tableName> 
	//drop <object> <name>
};

#endif
