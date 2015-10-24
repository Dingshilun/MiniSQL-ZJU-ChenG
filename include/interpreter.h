#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include "miniSQL.h"
#include "interface.h"
#include <set>
#include <vector>
#include <string>
#include <map>

using namespace std;
typedef struct AattrNode{
	bool isPrimary, isUnique;
	int lenth, type, offset;
	string attrName;
}attrNode;


class BreakException{};
class SyntaxException{
public:
	string errlog;
	SyntaxException(string a): errlog(a) {}
};


class Interpreter
{
public:
	Interpreter();
	void readCommand(bool cmd);
	Interface parse();
private:
	set<char> token;
	vector<string> statement;
	//CatalogManager catalog;
	map<string, int> op;
private:
	int split(string line);
	void refresh();
	void processCondition(string table, vector<string> &statement, int index, Interface &interface);
};

#endif