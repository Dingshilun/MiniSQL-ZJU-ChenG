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
	AattrNode() :isPrimary(0), isUnique(0), hasIndex(0), length(0), type(0), offset(0), attrName("")
	{
	
	}
}attrNode;
#endif
