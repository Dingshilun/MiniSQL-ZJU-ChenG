#ifndef _API_H_
#define _API_H_
#include"interface.h"
#include"catalogManager.h"
#include"IndexManager.h"
#include"new_recordManager.h"
#include"string"
#include"iostream"
#include<map>
typedef struct aav
{
	bool isPrimary, isUnique, isNull;
	int length, type;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
	string str_value;//if attribute is string
	int int_value;//if attribute is int
	float float_value;
}attrAndvalue;
class API
{
private:
	catalogManager& catlog;
	IndexManager indexmanager;
	map<string, list<attrNode>*> AttrTable;
	recordManager recordmanager;
	int createTable(string tablename, vector<attrNode> attributes);
	int createIndex(string tablename, string indexname, string attribute,int column);
	int Insert(string tablename, vector<Union> attlist);
	int Delete(string tablename, vector<TreeNode> conditions);
	int dropTable(string tablename);
	int dropIndex(string indexname,string tablename);
	int Select(string tablename, vector<TreeNode> conditions);
	attrNode getAttName(string tablename, int column);
public:
	API(catalogManager&tmp) :catlog(tmp) {}
	~API(){
		
	}
	int exec(Interface inter);

};

#endif
