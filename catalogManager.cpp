#include "stdafx.h"
#include "catalogManager.h"
#include <fstream>
#include <iostream>
#include <exception>

using namespace std;
catalogManager::catalogManager()
{
	readCatalog();
}

catalogManager::~catalogManager()
{
	writeCatalog();
}

string makeIndexName(string tablename, string indexname)
{
	return tablename + "-" + indexname + ".index";
}
string makeConfigName(string tablename, string indexname)
{
	return tablename + "-" + indexname + ".config";
}

string makeTableName(string tablename)
{
	return tablename;
}

string makeBareIndexName(string tablename, string indexname)
{
	return tablename + "-" + indexname;
}

void catalogManager::readCatalog()
{
	fstream logfile;
	logfile.open("tableLog.log", ios::in);
	if (logfile.good())
	{
		string s;
		logfile >> s;
		if (s.length() != 0)
		{
			logfile.seekg(ios::beg);
			try
			{
				copy(istream_iterator<TableNode>(logfile), istream_iterator<TableNode>(), back_inserter(this->tableList));

			}
			catch (const length_error& e)
			{
				cerr << e.what() << endl;
			}
			logfile.close();
		}
	}
	logfile.open("indexLog.log", ios::in);
	if (logfile.good())
	{
		copy(istream_iterator<indexNode>(logfile), istream_iterator<indexNode>(), back_inserter(this->indexList));
		logfile.close();
	}
}

void catalogManager::writeCatalog()
{
	fstream logfile;
	logfile.open("tableLog.log", ios::out);
	if (logfile.good())
	{
		copy(this->tableList.begin(), this->tableList.end(), ostream_iterator<TableNode>(logfile));
		logfile.close();
	}
	logfile.open("indexLog.log", ios::out);
	if (logfile.good())
	{
		copy(this->indexList.begin(), this->indexList.end(), ostream_iterator<indexNode>(logfile));
		logfile.close();
	}
}

bool catalogManager::createTable(string tablename, list<attrNode> attrlist)
{
	fstream tablefile;
	tablefile.open(makeTableName(tablename), ios::in);
	if (tablefile.good())//already exists
	{
		return false;
	}
	else
	{
		tablefile.close();
		tablefile.open(makeTableName(tablename), ios::out);
		tablefile.close();
		TableNode node(tablename, attrlist, 0);
		this->tableList.push_back(node);
		return true;
	}
}

bool catalogManager::createIndex(string indexname, string tablename, int columns)
{
	fstream indexfile, configfile;
	indexfile.open(makeIndexName(tablename, indexname), ios::in);
	configfile.open(makeConfigName(tablename, indexname), ios::in);
	if (indexfile.good() && configfile.good())//already exists
	{
		return false;
	}
	else
	{
		indexfile.close();
		indexfile.open(makeIndexName(tablename, indexname), ios::out);
		indexfile.close();
		configfile.close();
		configfile.open(makeConfigName(tablename, indexname), ios::out);
		configfile.close();
		//find attrName of column
		string attrname;
		list<TableNode>::iterator tite;
		list<AattrNode>::iterator aite;
		for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
		{
			if (tite->tableName == tablename)
			{
				aite = tite->attrList.begin();
				for (int i = 0; i < columns; ++i)
					++aite;
				attrname = aite->attrName;
				aite->hasIndex = true;//set hasIndex
				if (aite->isPrimary)
					indexname = string("$") + indexname;
			}
		}
		indexNode node(tablename, indexname, attrname, columns);
		this->indexList.push_back(node);
		return true;
	}
}

bool catalogManager::doesTableExist(string tablename)
{
	list<TableNode>::iterator tite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			return true;
		}
	}
	return false;
}

bool catalogManager::doesIndexExist(string indexname)
{
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end(); ++iite)
	{
		if (iite->indexName == indexname)
			return true;
	}
	return false;
}

indexNode catalogManager::findindex(string indexname, string tablename)
{
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end(); ++iite)
	{
		if (iite->indexName == indexname&&iite->tableName == tablename)
			return *iite;
	}
}

bool catalogManager::doesAttrExist(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return true;
			}
		}
	}
	return false;
}

bool catalogManager::isAttrUnique(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->isUnique)
					return true;
				else
					return false;
			}
		}
	}
	return -1;//not found
}

int catalogManager::getAttrType(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return aite->type;
			}
		}
	}
	return -1;//not found
}

int catalogManager::getAttrOffset(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return aite->offset;
			}
		}
	}
	return -1;//not found
}
int catalogManager::getAttrLength(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return aite->length;
			}
		}
	}
	return -1;//not found
}

list<attrNode> catalogManager::getAttrList(string tablename)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			return tite->attrList;
		}
	}
}

attrNode catalogManager::getAttrInfo(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return *aite;
			}
		}
	}
}

list<string> catalogManager::getIndexOfTable(string tablename)
{
	list<indexNode>::iterator iite;
	list<string> sv;
	for (iite = this->indexList.begin(); iite != this->indexList.end(); ++iite)
	{
		if (iite->tableName == tablename)
			sv.push_back(iite->indexName);
	}
	return sv;
}

string catalogManager::getIndexByAttrID(string tablename, int column)
{
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end(); ++iite)
	{
		if (iite->tableName == tablename && iite->column == column)
			return makeBareIndexName(tablename, iite->indexName);
	}
	return "";
}

string catalogManager::getIndexFileByIndexName(string indexname)
{
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end(); ++iite)
	{
		if (iite->indexName == indexname)
			return makeBareIndexName(iite->tableName, indexname);
	}
	return "";
}

int catalogManager::getRecordNum(string tablename)//返回条目数量
{
	list<TableNode>::iterator tite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			return tite->recordNum;
		}
	}
	return -1;
}

int catalogManager::getAttrNum(string tablename, string attrname)
{
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	int num = 0;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
			{
				if (aite->attrName == attrname)
					return num;
				++num;
			}
		}
	}
	return -1;//not found;
}
/*
bool catalogManager::deleteIndex(string indexname, string tablename)
{
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end();)
	{
		if (iite->tableName == tablename&&iite->indexName == indexname)
		{
			iite = this->indexList.erase(iite);
			string filename = makeIndexName(tablename, indexname);
			remove(filename.c_str());
			filename = makeConfigName(tablename, indexname);
			remove(filename.c_str());
		}
		else
		{
			++iite;
		}

	}
	return true;
}
*/

bool catalogManager::deleteIndex(string indexname)
{
	list<indexNode>::iterator iite;
	list<TableNode>::iterator tite;
	list<AattrNode>::iterator aite;
	for (iite = this->indexList.begin(); iite != this->indexList.end();)
	{
		if (iite->indexName == indexname)
		{
			for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
			{//reset hasIndex
				if (tite->tableName == iite->tableName)
				{
					for (aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
					{
						if (aite->attrName == iite->attribute)
							aite->hasIndex = false;
					}
				}
			}
			string filename = makeIndexName(iite->tableName, indexname);
			remove(filename.c_str());
			filename = makeConfigName(iite->tableName, indexname);
			remove(filename.c_str());
			iite = this->indexList.erase(iite);
		}
		else
		{
			++iite;
		}

	}
	return true;
}


bool catalogManager::deleteTable(string tablename)
{
	list<TableNode>::iterator tite;
	for (tite = this->tableList.begin(); tite != this->tableList.end();)
	{
		if (tite->tableName == tablename)
		{
			tite = this->tableList.erase(tite);
			string filename = makeTableName(tablename);
			remove(filename.c_str());
		}
		else
		{
			++tite;
		}
	}
	list<indexNode>::iterator iite;
	for (iite = this->indexList.begin(); iite != this->indexList.end();)
	{
		if (iite->tableName == tablename)
		{
			string filename = makeIndexName(tablename,iite->indexName);
			remove(filename.c_str());
			filename = makeConfigName(tablename, iite->indexName);
			remove(filename.c_str());
			iite = this->indexList.erase(iite);
		}
		else
		{
			++iite;
		}
	}
	return true;
}

bool catalogManager::recordDelete(string tablename, int num)//删除条目数量，删除成功返回1
{
	list<TableNode>::iterator tite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			tite->recordNum -= num;
			return true;
		}
	}
	return -1;//not found;
}
bool catalogManager::recordAdd(string tablename, int num)
{
	list<TableNode>::iterator tite;
	for (tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		if (tite->tableName == tablename)
		{
			tite->recordNum += num;
			return true;
		}
	}
	return -1;//not found;
}

string isTrue(bool x){
	if (x)
		return "true";
	else return "false";
}

void catalogManager::showTables()
{
	
	for (list<TableNode>::iterator tite = this->tableList.begin(); tite != this->tableList.end(); ++tite)
	{
		cout << "==============================================================================" << endl;
		int column = 0;
		cout << "table: " << tite->tableName << endl;
		cout << endl;
		
		cout << "attribute" << '\t' << "Primary" << '\t' << "Unique" << '\t' << "Type" << "\t\t" << "index" << endl;
		cout << "------------------------------------------------------------------------------" << endl;
		for (list<attrNode>::iterator aite = tite->attrList.begin(); aite != tite->attrList.end(); ++aite)
		{
			cout << aite->attrName << (aite->attrName.size() < 8 ? "\t" : "") <<  '\t' << isTrue(aite->isPrimary) << '\t' << isTrue(aite->isUnique) << '\t';
			switch (aite->type){
			case 3:
				cout << "float" << "\t\t"; break;
			case 1:
				cout << "int" <<"\t\t"; break;
			case 2:
				cout << "char(" << aite->length << ")" << '\t' << (aite->length > 9 ? "" : "\t"); break;
			}
			if (aite->hasIndex)
				cout << getIndexByAttrID(tite->tableName, column);
			else cout << "none";
			cout << endl;
			column++;
		}
		cout << "==============================================================================" << endl;
	}
	cout << endl;
}

void catalogManager::showIndexes(){
	cout << "==============================================================================" << endl;
	cout << "index" << "\t\t" << "table" << "\t\t" << "attribute" << endl;
	cout << "------------------------------------------------------------------------------" << endl;
	for (list<indexNode>::iterator iter = this->indexList.begin(); iter != this->indexList.end(); ++iter){
		cout << iter->indexName << '\t' << (iter->indexName.size() >= 8 ? "" : "\t") << iter->tableName << '\t' << (iter->tableName.size() >= 8 ? "" : "\t") << iter->attribute << endl;
	}
	cout << "==============================================================================" << endl;
}