#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include "MINISQL.h"
#include "API.h"
#include "catalogManager.h"
#include "interface.h"
#include "attrNode.h"
#include <set>
#include <vector>
#include <string>
#include <map>
using namespace std;
class ShowSomething{
public:
	string thing;
	ShowSomething(string a):thing(a){}
};
class BreakException{};
class SyntaxException{
public:
	string errlog;
	SyntaxException(string a) : errlog(a) {}
};


class Interpreter
{
public:
	Interpreter();
	void readCommand(bool cmd);
	void pipeline();
	Interface parse();
private:
	set<char> token;
	vector<string> statement;

	map<string, int> op;
	bool loop;
	bool cmd;
	int level;
	catalogManager catalog;
	API api;
private:
	int split(string line);
	void refresh();
	void processCondition(string table, vector<string> &statement, int index, Interface &interface);
};

#endif