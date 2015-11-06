#ifndef _attrNode_h_
#define _attrNode_h_
#include<string>
#include"miniSQL.h"
using namespace std;
typedef struct AattrNode
{
	bool isPrimary, isUnique, hasIndex;
	int length, type, offset;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
	string attrName;
}attrNode;
#endif
