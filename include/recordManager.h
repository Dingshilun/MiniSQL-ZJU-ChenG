#ifndef __recordManager_H__
#define __recordManager_H__

#include <string>
#include <iostream>
#include <map>
#include "interface.h"
#include "attrNode.h"
#include "bufferManager.h"
using namespace std;

/*
typedef struct aav
{
	bool isPrimary, isUnique, isNull;
	int length, type;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
	string str_value;//if attribute is string
	int int_value;//if attribute is int
	float float_value;
}attrAndvalue;
方便测试用加上的
*/

struct index_info{
	int num;
	int offset;
};

struct attrValue
{
	string attrName;
	index_info ii;
	Union value;
	int type;	//1：int ，2：char* ， 3：float

	attrValue(int type, string attrName,int num,int offset):type(type),attrName(attrName){
		ii.num = num;	ii.offset = offset;
	}
};	/*seletct_attr返回的类*/

struct deleted_node
{
	int type;
	int att_id;
	Union value; 
	deleted_node(int type, int att_id):type(type),att_id(att_id){}
};	/*delete返回的类*/


class recordManager : public bufferManager {

private:
	int count ;
	vector < vector < deleted_node > > del_ind;
	vector <attrValue> sel_attr;	/*返回值的引用*/
	index_info insert_ii;

public:
	recordManager();
	~recordManager();

	void judge(bool &notMatch, vector <TreeNode> &v_tn , vector <attrNode> &v_an, bufferNode &temp_bn, int &offset);
	void print(vector <attrNode> &v_an , bufferNode &temp_bn, int offset);
	void delAndGetValue(vector < vector < deleted_node > > &del_ind, vector<attrNode> &v_an, vector<int> &v_indexNum, map<int,int> &m, bufferNode &temp_bn, int &offset);

	vector <attrValue> & select_attr(string table_name, vector <attrNode> &v_an ,int num ); 
		/* 返回某个属性的所有值 */
	int select(string table_name, int r_length, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info> &join_ii);
		/* v_an 提供该表所有属性信息 */
	index_info &insert(string table_name, int r_length, vector<attrAndvalue> &v_aav );
		/*	*/
	vector < vector < deleted_node > >& delete_tuple (string table_name, int r_length, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info>  &join_ii );
		/* 根据不同属性返回若干aav的list */
	int getCount(){
		return count;
	}
};

#endif
