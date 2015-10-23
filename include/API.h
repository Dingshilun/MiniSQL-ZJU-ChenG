#ifndef _API_H_
#define _API_H_
#include"interface.h"
#include"catalogManager.h"
#include"IndexManager.h"
#include"string"
#include"iostream"
class API
{
private:
	catalogManager& catlog;
	IndexManager *indexmanager;
	/*recordmanager*/
	int createTable(string tablename, list<attrNode> attributes);
	int createIndex(string tablename, string indexname, int column);
	int Insert(string tablename, list<attrNode> attlist);
	int Delete(string tablename, vector<TreeNode> conditions);
	int dropTable(string tablename);
	int dropIndex(string indexname);
	int Select(string tablename, vector<TreeNode> conditions);
public:
	API(catalogManager&tmp) :catlog(tmp) {}
	~API(){
		if (indexmanager) delete indexmanager;
	}
	int exec(Interface inter);

};

#endif
