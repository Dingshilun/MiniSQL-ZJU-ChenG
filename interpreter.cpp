#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

Interpreter::Interpreter() :api(catalog)
{
	token.insert('('); token.insert(')'); token.insert(','); token.insert(';');
	token.insert(' '); token.insert('\''); token.insert('\"');
	token.insert('>'); token.insert('<'); token.insert('=');
	token.insert('\t'); token.insert('\n'); token.insert('\r');
	op["<"] = LT; op[">"] = GT; op["="] = EQU; op["<="] = LE; op[">="] = GE; op["<>"] = NE;
	loop = true;
	cmd = true;
	level = 0;
	refresh();
}

void Interpreter::pipeline()
{
	while (loop){
		refresh();
		try{
			readCommand(cmd);
			if (statement.size() == 0)
				continue;
			/*for (int i = 0; i < statement.size(); ++i)
				cout << statement[i] << ' ';
			cout << endl;
			*/Interface interface = parse();
			api.exec(interface);

		}
		catch (SyntaxException ex)
		{
			cout << "Syntax Error: " << ex.errlog << endl << endl;
			for (int i = 0; i < statement.size(); ++i)
				cout << statement[i] << ' ';
			cout << endl;
		}
		catch (string file)
		{
			streambuf *backup;
			ifstream fin;
			fin.open(file.c_str());
			backup = cin.rdbuf();   // back up cin's streambuf  
			cin.rdbuf(fin.rdbuf()); // assign file's streambuf to cin 
			cmd = false;
			level++;
			pipeline();
			loop = true;
			level--;
			if (level == 0)
				cmd = true;
			cin.rdbuf(backup);     // restore cin's original streambuf  
			fin.close();
		}
		catch (ShowSomething e)
		{
			string thing = e.thing;
			if (thing == "tables")
				catalog.showTables();
			else if (thing == "indexes")
				catalog.showIndexes();
		}
		catch (BreakException be)
		{
			break;
		}
	}



}

void Interpreter::refresh() //clear statement
{
	statement.clear();
}

void Interpreter::readCommand(bool cmd)	//if a statement is 
{
	if (cmd)
		cout << "miniSQL> ";
	string line;
	getline(cin, line);
	if (cin.eof()) loop = false;
	if (line.size() == 0)
		return;
	//remove the last invisible characters	
	int trim = line.size() - 1;
	while (line[trim] == '\n' || line[trim] == '\t'
		|| line[trim] == '\r' || line[trim] == ' ')
		trim--;
	line = line.substr(0, trim + 1);
	//to lower case;
	for (int i = 0; i < line.size(); ++i)
	if (line[i] <= 'Z' && line[i] >= 'A')
		line[i] += 32;
	int place; //place of ';'
	while ((place = split(line)) == -1){ //doen't meet a ';'
		if (cmd)
			cout << "      -> ";
		getline(cin, line);
	}
	if (cin.eof()) loop = false;
	if (place != line.size()) //there may be more than one ';'s
		throw SyntaxException("there should be exactly one ';' locating at the end of a statement.");
}

int Interpreter::split(string line)
{
	int cnt = 0;
	for (int i = 0; i < line.size();){
		//partition
		string t = "";
		int j = i;
		while (j < line.size() && !token.count(line[j])) ++j;
		t = line.substr(i, j - i);
		if (t.size())
			statement.push_back(t);
		if (j >= line.size()) break;

		//process the tokens
		i = j + 1;
		t = "";
		t += line[j];
		if (t == " " || t == "\n" || t == "\t" || t == "\r")
			continue;
		else if (t == "<" && j + 1 < line.size()){
			if (line[j + 1] == '>' || line[j + 1] == '='){	//<> or <=
				t += line[j + 1];
				++i;
			}
		}
		else if (t == ">" && j + 1 < line.size()){
			if (line[j + 1] == '='){	//>=
				t += line[j + 1];
				++i;
			}
		}
		else if (t == "\'"){	//'something'
			j++;
			while (j < line.size() && line[j] != '\''){
				if (line[j] == '\\'){
					if (j + 1 < line.size()){
						j++;
					}
				}
				t += line[j];
				j++;
			}
			if (j != line.size()) t += '\'';
			else
				throw SyntaxException("quotes should be in the same line.");
			i = j + 1;
		}
		else if (t == "\""){	//"something"->'something'
			j++;
			while (j < line.size() && line[j] != '\"'){
				if (line[j] == '\\'){
					if (j + 1 < line.size()){
						j++;
					}
				}
				t += line[j];
				j++;
			}
			if (j != line.size()) t += '\'';
			else
				throw SyntaxException("quotes should be in the same line.");
			i = j + 1;
		}
		if (t == ";") return i;
		if (t.size())
			statement.push_back(t);
	}
	return -1;
}

Interface Interpreter::parse()
{
	Interface interface;
	if (statement[0] == "create")
	{
		if (statement.size() < 2 || statement[1] != "table" && statement[1] != "index")
			throw SyntaxException("fail to create something.");
		//create index
		if (statement[1] == "index"){
			if (statement.size() != 8 || statement[3] != "on" || statement[5] != "(" || statement[statement.size() - 1] != ")")
				throw SyntaxException("\'create index\' should be in the format: create index <index_name> on <talbe_name> ( <colume_name> ).");
			if (!catalog.doesTableExist(statement[4]))
				throw SyntaxException("table \'" + statement[4] + "\' not found.");
			if (catalog.doesIndexExist(statement[2]))
				throw SyntaxException("index \'" + statement[2] + "\' has already existed.");
			if (!regex_match(statement[2], regex("[_|a-z|A-Z](\\w*)")))
				throw SyntaxException(string("\'") + statement[2] + "\' isn't a legal index name.");
			if (!catalog.doesAttrExist(statement[4], statement[6]))
				throw SyntaxException("column \'" + statement[6] + "\' not found");
			attrNode attr = catalog.getAttrInfo(statement[4], statement[6]);
			if (attr.isPrimary)
				throw SyntaxException(statement[6] + " is a primary key, and a default index has been created.");
			if (!attr.isUnique)
				throw SyntaxException(statement[6] + " isn't a unique key.");
			interface.setOperation(MINI_CREATE);
			interface.setObject(MINI_INDEX);
			interface.setTableName(statement[4]);
			interface.setIndexName(statement[2]);
			interface.setColumn(catalog.getAttrNum(statement[4], statement[6]));
		}
		//create table
		else if (statement[1] == "table"){
			if (statement.size() < 7 || statement[3] != "(" || statement[statement.size() - 1] != ")")
				throw SyntaxException("\'create table\' should be in the format: create table <table_name> ( <attr_list> [, primary key(attr_name)]).");
			if (catalog.doesTableExist(statement[2]))
				throw SyntaxException("table \'" + statement[2] + "\' has already existed.");
			if (!regex_match(statement[2], regex("[_|a-z|A-Z](\\w*)")))
				throw SyntaxException(string("\'") + statement[2] + "\' isn't a legal table name.");
			vector<vector<string> > attributes;
			//split to get attributes;
			for (int i = 4; i < statement.size() - 1;){
				vector<string> attribute;
				while (i < statement.size() - 1 && statement[i] != ","){
					attribute.push_back(statement[i]);
					++i;
				}
				if (statement[i] == ","){
					++i;
					if (i == statement.size() - 1 || statement[i] == ",")
						throw SyntaxException("there exists null attributes.");
				}
				attributes.push_back(attribute);
			}
			set<string> attrCnt;
			for (int i = 0; i < attributes.size(); ++i){
				vector<string> &attribute = attributes[i];
				if (attribute.size() == 0)
					throw SyntaxException("there exists null attributes.");
				//primary key
				if (attribute[0] == "primary" && i == attributes.size() - 1){
					if (attribute.size() != 5 || attribute[1] != "key" || attribute[2] != "(" || attribute[4] != ")")
						throw SyntaxException("\'primary key\' should be in the format: primary key ( <attr_name> ).");
					if (!attrCnt.count(attribute[3]))
						throw SyntaxException(attribute[3] + " undefined before.");
					int index = interface.findIndexOfDef(attribute[3]);
					interface.setPrimaryKeyIndex(index);
				}
				//normal keys
				else{
					if (attribute.size() < 2)
						throw SyntaxException("\'attribute\' should be in the format: <attr_name> <type>.");
					if (attrCnt.count(attribute[0]))
						throw SyntaxException(string("\'") + attribute[0] + "\' multiple defined.");
					//test whether the identifier is legal
					regex pattern("[_|a-z|A-Z](\\w*)");
					if (!regex_match(attribute[0], pattern))
						throw SyntaxException(string("\'") + attribute[0] + "\' isn't a legal attribute name.");

					attrNode attr;
					attr.attrName = attribute[0];
					if (attribute[1] == "int"){
						if (attribute.size() == 3 && attribute[2] != "unique")
							throw SyntaxException("INT attribute should be in the format: <attr_name> int [unique].");
						else if (attribute.size() == 3 && attribute[2] == "unique")
							attr.isUnique = true;
						else if (attribute.size() != 2)
							throw SyntaxException("INT attribute should be in the format: <attr_name> int [unique].");
						attr.type = MINI_INT;
					}
					else if (attribute[1] == "float"){
						if (attribute.size() == 3 && attribute[2] != "unique")
							throw SyntaxException("FLOAT attribute should be in the format: <attr_name> float [unique].");
						else if (attribute.size() == 3 && attribute[2] == "unique")
							attr.isUnique = true;
						else if (attribute.size() != 2)
							throw SyntaxException("FLOAT attribute should be in the format: <attr_name> float [unique].");
						attr.type = MINI_FLOAT;
					}
					else if (attribute[1] == "char"){
						if (attribute.size() == 6 && attribute[5] != "unique")
							throw SyntaxException("CHAR attribute should be in the format: <attr_name> char(<len>) [unique].");
						else if (attribute.size() == 6 && attribute[5] == "unique")
							attr.isUnique = true;
						else if (attribute.size() != 5 || attribute[2] != "(" || attribute[4] != ")")
							throw SyntaxException("CHAR attribute should be in the format: <attr_name> char(<len>) [unique].");
						regex integer("\\d+");
						if (!regex_match(attribute[3], integer) || attribute[3].size() > 4)
							throw SyntaxException(string("\'") + attribute[3] + "\' should be a positive integer less than 10000.");
						int len = atoi(attribute[3].c_str());
						if (len == 0)
							throw SyntaxException(string("\'") + attribute[3] + "\' should be a positive integer less than 10000.");
						attr.type = MINI_STRING;
						attr.length = len;
					}
					else
						throw SyntaxException(string("\'") + attribute[1] + "\' unknown type.");
					attrCnt.insert(attribute[0]);
					interface.addDefinition(attr);
				}
			}
			interface.setOperation(MINI_CREATE);
			interface.setObject(MINI_TABLE);
			interface.setTableName(statement[2]);
		}
	}
	else if (statement[0] == "drop")
	{
		//drop <object> <name>
		//exactly 3 words
		if (statement.size() != 3)
			throw SyntaxException("too many or too few words in this statement.");
		interface.setOperation(MINI_DROP);

		//drop table
		if (statement[1] == "table"){
			if (!catalog.doesTableExist(statement[2]))
				throw SyntaxException(string("table \'") + statement[2] + "\' not found.");
			if (!regex_match(statement[2], regex("[_|a-z|A-Z](\\w*)")))
				throw SyntaxException(string("\'") + statement[2] + "\' isn't a legal table name.");
			else{
				interface.setObject(MINI_TABLE);
				interface.setTableName(statement[2]);
			}
		}
		//drop index
		else if (statement[1] == "index"){
			if (!catalog.doesIndexExist(statement[2]))
				throw SyntaxException(string("index \'") + statement[2] + "\' not found.");
			if (!regex_match(statement[2], regex("[_|a-z|A-Z](\\w*)")))
				throw SyntaxException(string("\'") + statement[2] + "\' isn't a legal table name that can be drop.");
			interface.setObject(MINI_INDEX);
			interface.setIndexName(statement[2]);
			
		}
		else
			throw SyntaxException(string("unknown identifier \'") + statement[1] + "\'.");
	}
	else if (statement[0] == "select")
	{
		if (statement.size() < 4 || statement[1] != "*" || statement[2] != "from"
			|| (statement.size() > 4 && (statement[4] != "where" || statement.size() == 5)))
			throw SyntaxException("\'select\' should be in the format: select * from <table_name> [where <conditions>]");
		if (!catalog.doesTableExist(statement[3]))
			throw SyntaxException(string("table \'") + statement[3] + "\' not found.");
		if (statement.size() > 4)	//has where clause
			processCondition(statement[3], statement, 5, interface);
		interface.setOperation(MINI_SELECT);
		interface.setObject(MINI_TABLE);
		interface.setTableName(statement[3]);
	}
	else if (statement[0] == "insert")
	{
		if (statement.size() < 7 || statement[1] != "into" || statement[3] != "values"
			|| statement[4] != "(" || statement[statement.size() - 1] != ")")
			throw SyntaxException("\'insert\' should be in the format: insert into <table_name> values (<value_list>)");
		if (!catalog.doesTableExist(statement[2]))
			throw SyntaxException(string("table \'") + statement[2] + "\' not found.");
		vector<string> values;
		//split to get values
		for (int i = 5; i < statement.size() - 1; i += 2){
			if (statement[i] == ",")
				throw SyntaxException("if you want to insert a null value, please explicitly use \'null\'.");
			values.push_back(statement[i]);
			if (i + 1 < statement.size() - 1 && statement[i + 1] != ",")
				throw SyntaxException("there must be a ',' between two values.");
			if (i + 1 == statement.size() - 2)
				throw SyntaxException("if you want to insert a null value, please explicitly use \'null\'.");
		}
		//get attribute list
		list<attrNode> attribute = catalog.getAttrList(statement[2]);
		if (values.size() != attribute.size())
			throw SyntaxException("the number of values doesn't match the table.");
		int i = 0;
		//test each value;
		for (list<attrNode>::iterator iter = attribute.begin(); iter != attribute.end(); ++iter, ++i){
			Union u;
			if (values[i] == "null"){
				u.isNull = true;
				interface.addData(u);
				continue;
			}
			switch (iter->type){
			case MINI_INT:
			{
							 // //contains only numbers
							 regex pattern("((-|\\+)?)\\d+");
							 if (!regex_match(values[i], pattern))
								 throw SyntaxException(iter->attrName + " is defined as an INT while \'" + values[i] + "\' isn't.");

							 //number is in range
							 long long value = 0;
							 long long range = 1LL << 32 - 1;
							 int j = 0;
							 if (values[i][0] == '-'){
								 j++;
								 range++;
							 }
							 for (; j < values[i].size(); ++j){
								 value = value * 10 + values[i][j] - 48;
								 if (value > range) break;
							 }
							 if (j < values[i].size())
								 throw SyntaxException(values[i] + " is out of range of INT.");
							 u.n = (int)value;
							 break;
			}
			case MINI_FLOAT:
			{
							   regex pattern("(-|\\+)?\\d+(.\\d+)?");
							   if (!regex_match(values[i], pattern))
								   throw SyntaxException(iter->attrName + " is defined as a FLOAT while \'" + values[i] + "\' isn't.");
							   double f = atof(values[i].c_str());
							   if (f == HUGE_VAL)
								   throw SyntaxException("number out of range.");
							   u.f = (float)f;
							   break;
			}
			case MINI_STRING:
			{
								if (values[i].size() <= 1 || values[i][0] != '\'' || values[i][values[i].size() - 1] != '\'')
									throw SyntaxException(values[i] + " can't be identified as a string.");
								string s = values[i].substr(1, values[i].size() - 2);
								attrNode attr = catalog.getAttrInfo(statement[2], iter->attrName);
								int len = attr.length;
								if (s.size() > len)
									throw SyntaxException(values[i] + " is oversized.");
								u.s = s;
			}
			}
			interface.addData(u);
		}
		interface.setOperation(MINI_INSERT);
		interface.setObject(MINI_TABLE);
		interface.setTableName(statement[2]);
	}
	else if (statement[0] == "delete")
	{
		if (statement.size() < 3 || statement[1] != "from"
			|| (statement.size() > 3 && (statement[3] != "where" || statement.size() == 4)))
			throw SyntaxException("\'delete\' should be in the format: delete from <table_name> [where <conditions>]");
		if (!catalog.doesTableExist(statement[2]))
			throw SyntaxException(string("table \'") + statement[2] + "\' not found.");
		if (statement.size() > 3)
			processCondition(statement[2], statement, 4, interface);
		interface.setOperation(MINI_DELETE);
		interface.setObject(MINI_TABLE);
		interface.setTableName(statement[2]);
	}
	else if (statement[0] == "quit")
	{
		if (statement.size() != 1)
			throw SyntaxException("too many words afer \'quit\'.");
		else
			throw BreakException();
	}
	else if (statement[0] == "execfile")
	{
		if (statement.size() != 2)
			throw SyntaxException("\'execfile\' should be in the format: execfile <file_name>.");
		ifstream fin(statement[1]);
		if (!fin)
			throw SyntaxException("cannot find file " + statement[1]);
		else{
			throw statement[1];
		}
	}
	else if (statement[0] == "show")
	{
		if (statement.size() != 2)
			throw SyntaxException("\'show tables\' or \'show indexes\'.");
		if (statement[1] == "tables"){
			throw ShowSomething("tables");
		}
		else if (statement[1] == "indexes"){
			throw ShowSomething("indexes");
		}
		else
			throw SyntaxException("\'show tables\' or \'show indexes\'.");
	}
	else
	{
		throw SyntaxException(string("unknown operation \'") + statement[0] + "\'.");
	}
	return interface;
}

void Interpreter::processCondition(string table, vector<string> &statement, int index, Interface &interface)
{
	vector<string> condition;
	while (index < statement.size()){
		condition.clear();
		//split to get a condtion
		while (index < statement.size() && statement[index] != "and"){
			condition.push_back(statement[index]);
			index++;
		}
		index++;
		if (condition.size() == 0 || index == statement.size())
			throw SyntaxException("there exists null condition in the statement");
		if (condition.size() != 3 || !op.count(condition[1]))
			throw SyntaxException("condition should be in format: <variable> <op> <value>.");
		if (!catalog.doesAttrExist(table, condition[0]))
			throw SyntaxException(string("column \'") + condition[0] + "\' not found.");
		//get attribute type
		int type = catalog.getAttrType(table, condition[0]);
		Union u;
		if (condition[2] == "null"){
			u.isNull = true;
			if (condition[1] != "=" || condition[1] != "<>")
				throw SyntaxException("Cannot compare a value with null using <, >, <= or >=.");
		}
		else switch (type){
		case MINI_INT:
		{
						 // //contains only numbers
						 regex pattern("((-|\\+)?)\\d+");
						 if (!regex_match(condition[2], pattern))
							 throw SyntaxException(condition[0] + " is defined as an INT while \'" + condition[2] + "\' isn't.");

						 //number is in range
						 long long value = 0;
						 long long range = 1LL << 32 - 1;
						 int i = 0;
						 if (condition[2][0] == '-'){
							 i++;
							 range++;
						 }
						 for (; i < condition[2].size(); ++i){
							 value = value * 10 + condition[2][i] - 48;
							 if (value > range) break;
						 }
						 if (i < condition[2].size())
							 throw SyntaxException(condition[2] + " is out of range of INT.");
						 u.n = (int)value;
						 break;
		}
		case MINI_FLOAT:
		{
						   regex pattern("(-|\\+)?\\d+(.\\d+)?");
						   if (!regex_match(condition[2], pattern))
							   throw SyntaxException(condition[0] + " is defined as a FLOAT while \'" + condition[2] + "\' isn't.");
						   double f = atof(condition[2].c_str());
						   if (f == HUGE_VAL)
							   throw SyntaxException("number out of range.");
						   u.f = (float)f;
						   break;
		}
		case MINI_STRING:
		{
							if (condition[2].size() <= 1 || condition[2][0] != '\'' || condition[2][condition[2].size() - 1] != '\'')
								throw SyntaxException(condition[2] + " can't be identified as a string.");
							string s = condition[2].substr(1, condition[2].size() - 2);
							u.s = s;
		}
		}
		TreeNode t;
		t.id = catalog.getAttrNum(table, condition[0]);
		t.type = type;
		t.op = op[condition[1]];
		t.value = u;
		interface.addCondition(t);
	}
}