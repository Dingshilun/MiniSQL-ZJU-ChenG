#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

struct Info{
	int operation;	//0: error detected
					//1: create table
					//2: drop table
					//3: create index
					//4: drop index
					//5: select
					//6: insert
					//7: delete
					//8: quit
	string name; 	//table name or index name
	void *head;		//cast when used
};
union Union{
		char *s;
		int n;
		float f;
};
struct TreeNode{
	string var;
	int op; //0: =; 1: <>; 2: <; 3: >; 4: <=; 5: >=;
	Union value;
	TreeNode *next;
};

struct Definiton{
	string var;
	int type; //1: int; 2: char; 3: float
	int len;
};
struct TableNode{
	vector<Definition> vec;
	int pkey = -1;
};

struct IndexNode{
	string table;
	string column;
};

class interpreter
{
public:
	interpreter();
	void initialize(); //initialize the catalog when first opened
	Info parse(string statement) /*throws*/ ;
	void invoke(Info info);
	/*
	* 需要：
	* 判断table、index是否存在的函数from Catalog
	* 或者直接创建、看返回值
	* 创建表：会传入TableNode的指针给Catalog
	* 创建 Index：会传入IndexNode的指针给Catalog（或者API）
	* select：TreeNode给API
	* drop：给Catalog（参数反正有名字就行）
	* insert: 传Union[]或vector<Union>给API
	* delete: 穿TreeNode给API


	*/
	void clear(Info info);
};

#endif