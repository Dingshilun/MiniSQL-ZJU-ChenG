#ifndef __recordManager_H__
#define __recordManager_H__

#include <string>

#include "bufferManager.h"
#include "interface.h"


struct index_info{
	int num;
	int offset;
};

struct attrValues
{
	string attrName;
	index_info ii;
	Union value;
	int type;	//1：int ，2：char* ， 3：float
};	/*seletct_attr返回的类*/

struct deleted_node
{
	int type;
	int att_id;
	Union value; 
};	/*delete返回的类*/

/* 
	typedef struct AattrNode
		{
			bool isPrimary,isUnique;
			bool hasIndex;
			int length,type,offset;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
			string attrName;			
		}attrNode;
*/

class recordManager : public bufferManager {

private:
	string table_name;
	int count ;
	vector < vector < deleted_node > >& del_ind;
	vector <attrValues>& sel_attr;	/*返回值的引用*/

public:
	recorcManager(string t_name):table_name(t_name){
	}
	~recorcManager();

	/*bool comp(const index_info &ii_a, const index_info &ii_b){
		if (ii_a.num != ii_b.num){
			return ii_a.num < ii_b.num;
		}
		else{
			return ii_a.offset < ii_b.offset;
		}
	}
	bool ii_equal(const index_info &ii_a, const index_info &ii_b){
		return ii_a.num == ii_b.num && ii_a.offset == ii_b.offset;
	}
	void intersect(vector<index_info> &join_ii,vector < vector <index_info> > &vv_ii);
		/*对向量集合合并排序*/

	vector <attrValues> & select_attr(string tablename, vector<attrNode> &attrNode , int num);
		/* 返回某个属性的所有值 */
	int select(string tablename, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info>  &vv_ii);
		/* v_an 提供该表所有属性信息 */
	index_info insert(string tablename, vector<attrAndvalue> &v_aav);
	vector < vector < deleted_node > >& delete_index (string tablename, vector<TreeNode> &v_tn, vector<attrNode> &v_an,  vector <index_info> &vv_ii );
		/* 根据不同属性返回若干aav的list */
};

#endif
