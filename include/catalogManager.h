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
    //���캯��	
    catalogManager();
	
    //��Ҫ�Ľӿ�
    //1�ɹ�,0ʧ��(����bool)��-1������(�������ֵint)
    bool createTable(string tablename,list<AttrNode> attrlist);
    bool createIndex(string indexname,string tablename,int columns);//columns:�ڼ��е�������Ҫindex
    bool doesTableExist(string tablename);
    bool doesIndexExist(string indexname);
    indexNode findindex(string indexname);
    bool doesAttrExist(string tablename,string attrname);
    bool isAttrUnique(string tablename,attrname);
    int getAttrType(string tablename,string attrname);
    int getAttrOffset(string tablename,string attrname);
    int getAttrLength(string tablename,string attrname);
    int getAttrNum(string tablename,string attrname);
    list<attrNode> getAttrList(string tablename);
    attrNode getAttrInfo(string tablename,string attrname);
    
    int getRecordNum(string tablename);//������Ŀ����
    bool deleteTable(string tablename);
    bool createIndex(string indexname,string tablename);
    bool recordDelete(string tablename,int num);//ɾ����Ŀ������ɾ���ɹ�����1
    bool recordAdd(string tablename,int num);

};
#endif
