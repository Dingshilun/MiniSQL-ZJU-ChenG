#ifndef _catalogManager_h_
#define _catalogManager_h_
#include<attrNode.h>
#include<indexNode.h>
#include<list>
using namespace std;
class catalogManager
{
    private 
	
    public
    //构造函数	
    catalogManager();
	
    //需要的接口
    //1成功,0失败(返回bool)，-1不存在(如果返回值int)
    bool createTable(string tablename,vector<attrNode> attrlist);
    bool createIndex(string indexname,string tablename,int columns);//columns:第几列的属性需要index
    bool doesTableExist(string tablename);
    bool doesIndexExist(string indexname);
    indexNode findindex(string indexname);
    bool doesAttrExist(string tablename,string attrname);
    bool isAttrUnique(string tablename,string attrname);
    int getAttrType(string tablename,string attrname);
    int getAttrOffset(string tablename,string attrname);
    int getAttrLength(string tablename,string attrname);
    int getAttrNum(string tablename,string attrname);
    vector<attrNode> getAttrList(string tablename);
    attrNode getAttrInfo(string tablename,string attrname);
    vector<string> getIndexOfTable(string tablename);
    string getIndexByAttrID(string tablename,int column);//不存在返回""空字符串

    int getRecordNum(string tablename);//返回条目数量
    bool deleteTable(string tablename);
    bool deleteIndex(string indexname,string tablename);
    bool recordDelete(string tablename,int num);//删除条目数量，删除成功返回1
    bool recordAdd(string tablename,int num);

};
#endif
