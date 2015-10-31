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
	int type;	//1��int ��2��char* �� 3��float
};	/*seletct_attr���ص���*/

struct deleted_node
{
	int type;
	int att_id;
	Union value; 
};	/*delete���ص���*/

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
	vector <attrValues>& sel_attr;	/*����ֵ������*/

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
		/*���������Ϻϲ�����*/

	vector <attrValues> & select_attr(string tablename, vector<attrNode> &attrNode , int num);
		/* ����ĳ�����Ե�����ֵ */
	int select(string tablename, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info>  &vv_ii);
		/* v_an �ṩ�ñ�����������Ϣ */
	index_info insert(string tablename, vector<attrAndvalue> &v_aav);
	vector < vector < deleted_node > >& delete_index (string tablename, vector<TreeNode> &v_tn, vector<attrNode> &v_an,  vector <index_info> &vv_ii );
		/* ���ݲ�ͬ���Է�������aav��list */
};

#endif
