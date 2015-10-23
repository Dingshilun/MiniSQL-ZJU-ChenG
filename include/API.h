#ifndef _API_H_
#define _API_H_
#include"interface.h"
#include"catalogManager.h"
#include"IndexManager.h"
#include"string"
#include"iostream"
typedef struct aav
{
	bool isPrimary, isUnique;
	int length, type;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
	string str_value;//if attribute is string
	int int_value;//if attribute is int
	float float_value;
}attrAndvalue;
class API
{
private:
	catalogManager& catlog;
	IndexManager *indexmanager;
	/*recordmanager*/
	int createTable(string tablename, list<attrNode> attributes);
	int createIndex(string tablename, string indexname, int column);
	int Insert(string tablename, list<attrAndvalue> attlist);
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
