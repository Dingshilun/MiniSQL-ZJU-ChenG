#ifndef _MINISQL_h_
#define _MINISQL_h_
#define MINI_FLOAT 3
#define MINI_INT 1
#define MINI_STRING 2
#define MINI_CREATE 0
#define MINI_DELETE 1
#define MINI_SELECT 2
#define MINI_DROP 3
#define MINI_INSERT 4
#define MINI_TABLE 0
#define MINI_INDEX 1
#define EQU 0
#define LT 1
#define GT 2
#define LE 3
#define GE 4
#define NE 5
#include<string>
using namespace std;
struct index_info{
	int num;
	int offset;
};
typedef struct aav
{
	bool isPrimary = 0, isUnique = 0, isNull = 0;
	int length, type;//type={MINI_FLOAT,MINI_INT,MINI_STRING}
	string str_value;//if attribute is string
	int int_value;//if attribute is int
	float float_value;
}attrAndvalue;
#endif
