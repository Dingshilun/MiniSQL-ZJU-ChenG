/*
 * 文件命名规则：表名.tbl   索引名-表名.idx
 */

#ifndef _catalogManager_h_
#define _catalogManager_h_
#include<attrNode.h>
//#include<indexNode.h>
#include <string>
#include <list>

class indexNode
{
public:
	indexNode()
	{
	}
	indexNode(std::string tablename,std::string indexname,std::string attrname,int column)
	{
		this->tableName = tablename;
		this->indexName = indexname;
		this->attribute = attrname;
		this->column = column;
	}

	std::string tableName;
	std::string attribute;
	std::string indexName;
	int column;

	friend std::ostream& operator<<(std::ostream& out, const indexNode& node)
	{
		out << node.tableName << '\t' << node.indexName << '\t' << node.attribute << '\t' << node.column;
		return out;
	}

	friend std::istream& operator>>(std::istream& in, indexNode& node)
	{
		in >> node.tableName >> node.indexName >> node.attribute >> node.column;
		return in;
	}
};

class TableNode
{
public:

	TableNode()
	{
		this->recordNum = 0;
	}
	TableNode(std::string tablename, std::list<attrNode> attrlist, int recordnum)
	{
		this->tableName = tablename;
		this->attrList = attrlist;
		this->recordNum = recordnum;
	}

	std::string tableName;
	std::list<attrNode> attrList;
	int recordNum;
	friend std::ostream& operator<<(std::ostream& out, const TableNode& node)
	{
		out << node.tableName << '\t' << node.recordNum << '\t' << node.attrList.size() << endl;
		for (list<attrNode>::const_iterator aite = node.attrList.begin(); aite != node.attrList.end(); ++aite)
		{
			out << aite->isPrimary << '\t' << aite->isUnique << '\t' << aite->hasIndex << '\t' << aite->length << '\t' << aite->type << '\t' << aite->offset << '\t' << aite->attrName << endl;
		}
		return out;
	}

	friend std::istream& operator>>(std::istream& in, TableNode& node)
	{
		in >> node.tableName >> node.recordNum;
		int s;
		in >> s;
		if (s < 0)
			return in;
		node.attrList.resize(s);
		for (std::list<attrNode>::iterator aite = node.attrList.begin(); aite != node.attrList.end(); ++aite)
		{
			in >> aite->isPrimary >> aite->isUnique >> aite->hasIndex >> aite->length >> aite->type >> aite->offset >> aite->attrName;
		}
		return in;
	}
};

class catalogManager
{
private:
	std::list<TableNode> tableList;
	std::list<indexNode> indexList;
public:
	//构造函数	
	catalogManager();
	virtual ~catalogManager();
	void readCatalog();
	void writeCatalog();
	
	//需要的接口
	//1成功,0失败(返回bool)，-1不存在(如果返回值int)
	bool createTable(string tablename, list<attrNode> attrlist);	
	bool createIndex(string indexname, string tablename, int columns);//columns:第几列的属性需要index
	bool doesTableExist(string tablename);
	bool doesIndexExist(string indexname, string tablename);
	indexNode findindex(string indexname,string tablename);
	bool doesAttrExist(string tablename, string attrname);
	bool isAttrUnique(string tablename, string attrname);
	int getAttrType(string tablename, string attrname);
	int getAttrOffset(string tablename, string attrname);
	int getAttrLength(string tablename, string attrname);
	int getAttrNum(string tablename, string attrname);
	list<attrNode> getAttrList(string tablename);
	attrNode getAttrInfo(string tablename, string attrname);
	list<string> getIndexOfTable(string tablename); 
	string getIndexByAttrID(string tablename, int column);//若不存在返回值为""
	int getRecordNum(string tablename);//返回条目数量
	bool deleteTable(string tablename);
	bool deleteIndex(string indexname, string tablename);
	bool recordDelete(string tablename, int num);//删除条目数量，删除成功返回1
	bool recordAdd(string tablename, int num);

};



#endif