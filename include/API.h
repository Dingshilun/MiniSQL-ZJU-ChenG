#ifndef _API_H_
#define _API_H_
#include"MINISQL.h"
#include"interface.h"
#include"catalogManager.h"
#include"IndexManager.h"
#include"recordManager.h"
#include"string"
#include"iostream"
#include<map>
using namespace std;

class API
{
private:
	catalogManager& catlog;
	IndexManager indexmanager;
	recordManager record_manager_;
	int createTable(string tablename, vector<attrNode> attributes);
	int createIndex(string tablename, string indexname, string attribute, int column);
	int Insert(string tablename, vector<Union> attlist);
	int Delete(string tablename, vector<TreeNode> conditions);
	int dropTable(string tablename);
	int dropIndex(string indexname, string tablename);
	int Select(string tablename, vector<TreeNode> conditions);
	attrNode getAttName(string tablename, int column);
public:
	API(catalogManager& tmp) :catlog(tmp) {}
	~API(){

	}
	int exec(Interface inter);

};

#endif
