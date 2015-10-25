#ifndef __recordManager_H__
#define __recordManager_H__

#include <string>

#include "bufferManager.h"
#include "interface.h"


struct index_info{
	int num;
	int offset;
};

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

public:
	recorcManager(string t_name):table_name(t_name){
	}
	~recorcManager();

	int select(int r_length, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector < vector <index_info> > &vv_ii);
		/* v_an 提供该表所有属性信息 */
	index_info insert(int r_length, vector<attrAndvalue> &v_aav);
	vector < vector <attrAndvalue > >& delete_index (int r_length, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector < vector <index_info> > &vv_ii );
		/* 根据不同属性返回若干aav的list */
};

#endif