#ifndef _catalogManager_h_
#define _catalogManager_h_
#include<attrNode.h>
//#include<indexNode.h>

#include <string>
#include <vector>


class indexNode
{
public:
	indexNode()
	{

	}
	indexNode(std::string tablename,std::string indexname,std::string attrname)
	{
		this->tableName = tablename;
		this->indexName = indexname;
		this->attribute = attrname;
	}

	std::string tableName;
	std::string attribute;
	std::string indexName;

	friend std::ostream& operator<<(std::ostream& out, const indexNode& node)
	{
		out << node.tableName << '\t' << node.indexName << '\t' << node.attribute;
		return out;
	}

	friend std::istream& operator>>(std::istream& in, indexNode& node)
	{
		in >> node.tableName >> node.indexName >> node.attribute;
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
	TableNode(std::string tablename, std::vector<attrNode> attrlist, int recordnum)
	{
		this->tableName = tablename;
		this->attrList = attrlist;
		this->recordNum = recordnum;
	}

	std::string tableName;
	std::vector<attrNode> attrList;
	int recordNum;
	friend std::ostream& operator<<(std::ostream& out, const TableNode& node)
	{
		out << node.tableName << '\t' << node.recordNum << '\t' << node.attrList.size() << endl;
		for (vector<attrNode>::const_iterator aite = node.attrList.begin(); aite != node.attrList.end(); ++aite)
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
		for (std::vector<attrNode>::iterator aite = node.attrList.begin(); aite != node.attrList.end(); ++aite)
		{
			in >> aite->isPrimary >> aite->isUnique >> aite->hasIndex >> aite->length >> aite->type >> aite->offset >> aite->attrName;
		}
		return in;
	}
};

class catalogManager
{
private:
	std::vector<TableNode> tableList;
	std::vector<indexNode> indexList;
public:
	//构造函数	
	catalogManager();
	virtual ~catalogManager();
	void readCatalog();
	void writeCatalog();
	
	//需要的接口
	//1成功,0失败(返回bool)，-1不存在(如果返回值int)
	bool createTable(string tablename, vector<attrNode> attrlist);
	
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
	vector<attrNode> getAttrList(string tablename);
	attrNode getAttrInfo(string tablename, string attrname);
	vector<string> getIndexOfTable(string tablename);
	int getRecordNum(string tablename);//返回条目数量
	bool deleteTable(string tablename);
	bool deleteIndex(string indexname, string tablename);
	bool recordDelete(string tablename, int num);//删除条目数量，删除成功返回1
	bool recordAdd(string tablename, int num);

};



#endif