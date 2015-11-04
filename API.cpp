#include"interface.h"
#include"catalogManager.h"
#include"IndexManager2.h"
#include"new_recordManager.h"
#include"API.h"
#include"string"
#include"iostream"
#include<algorithm>

using namespace std;
class CreationFailureOnCatalog{};
class CreationFailureOnBplus{};
class alreadyExist{};
bool compare(index_info a, index_info b)
{
	if (a.num < b.num)return true;
	else if (a.num == b.num) return a.offset < b.offset;
	return false;
}
void merge_index(vector<vector<index_info>>& index, vector<index_info>& result)
{
	//tested
	vector<vector<index_info>>::iterator it = index.begin();
	for (; it != index.end; it++)
	{
		sort(it->begin(), it->end(), compare);
	}
	bool flag = true;
	//merge the indexs
	vector<index_info> tmp;
	result.insert(result.begin(), index[0].begin(), index[0].end());
	vector<index_info>::iterator i;
	for (it = ++index.begin(); it != index.end(); it++)
	{
		vector<index_info>::iterator a, b;
		a = result.begin();
		b = it->begin();
		while (a != result.end() && b != it->end())
		{
			if (a->num == b->num&&a->offset == b->offset) { a++; b++; continue; }
			else if (compare(*a, *b))a = result.erase(a);
			else b++;
		}
		if (a != result.end()) while (a != result.end()) a = result.erase(a);
	}
}
attrNode API::getAttName(string tablename,int column)
{
	vector<attrNode>temp = catlog.getAttrList(tablename);
	return temp[column];
}
int API::exec(Interface inter)
{
	int op = inter.whichOperation();
	string tablename = inter.getTableName();
	switch (op){
		vector<attrNode> tableAttr;
	case MINI_CREATE://create index or table
		int type = inter.whichObject();
		switch (type)
		{
		case MINI_TABLE:
				createTable(tablename, inter.getDefinitions);
			break;
		case MINI_INDEX:
			try{
				int column = inter.indexOnWhichColumn();
				attrNode attr = getAttName(tablename, column);
				string attribute = attr.attrName;
				if (createIndex(tablename, inter.getIndexName(), attribute, inter.indexOnWhichColumn()))
				{
					/*select all the existed tuples in table and add them into the index*/
					/*addition needed*/
					cout << "Create Index successfully!" << endl;
				}
			}
			catch (){}
			break;
		}
		break;
	case MINI_DROP://drop table and index
		
		switch (inter.whichObject())
		{
		case MINI_TABLE:
				dropTable(tablename);
			break;
		case MINI_INDEX:
			/*delete the index */
			dropIndex(inter.getIndexName(),inter.getTableName());			
			break;
		}
		break;
	case MINI_SELECT:
		//edit needed
		int count=Select(tablename, inter.getConditions());
		cout << "The amount selected:" << count << endl;
		break;
	case MINI_INSERT:
		/*make a vector of attrAndvalue*/
		vector<Union> data = inter.getData();
		int result=Insert(tablename, data );
		switch (result){
		case -1:cout << "abort because of duplication!" << endl;
			break;
		case 1:cout << "insert successfully!" << endl;
		}
		break;
	case MINI_DELETE:
		int count = Delete(tablename, inter.getConditions);
		cout << "The amount deleted:" << count << endl;
		break;
	}
}
int API::createTable(string tablename, vector<attrNode> attributes)  //throw(alreadyExist,CreationFailureOnCatalog,)
{
	//return value 0 -1 1
	//0 means existed,-1 creation failure, 1 success
	int flag=catlog.doesTableExist(tablename);
	if (flag)
		return 0;//existed
	else
	{
		try
		{
			if (catlog.createTable(tablename, attributes))
			{
				vector<attrNode>::iterator it = attributes.begin();
				int column = 1;
				for (; it != attributes.end(); it++)
				{
					if (it->isPrimary)
					{
						//catlog.createIndex(tablename + ' ' + it->attrName, tablename, column);
						return (createIndex(tablename, tablename+'.'+it->attrName,it->attrName,column));
					}
					column++;
				}
				return -1;
			}
			else return -1;//creation failure
		}
		catch ()
		{
		}
	}
} 
int API::createIndex(string tablename, string indexname, string attribute,int column)throw(alreadyExist,CreationFailureOnBplus)
{
	int flag = catlog.doesIndexExist(indexname);
	if (flag)
		return 0;//existed
	else
	{	
		try
		{
			flag = 0;
			if (!catlog.createIndex(indexname, tablename, column))
				return -1;
			/*
			use b+tree to build the index
			*/
			indexname = catlog.getIndexByAttrID(tablename, column);
			/*attrNode attrPro = getAttName(tablename,column);
			switch (attrPro.type)
			{//for different type , create a corressponding index.
			case MINI_FLOAT:
				Target<float> tmp1(indexname);
				indexmanager.createIndex(tmp1, indexname);
				break;
			case MINI_INT:
				Target<int> tmp2(tablename, attribute);
				tmp2.setTable(tablename);
				tmp2.setAttribute(attribute);
				indexmanager.createIndex(tmp2, indexname);
				break;
			case MINI_STRING:
				Target<string> tmp3(tablename, attribute);
				tmp3.setTable(tablename);
				tmp3.setAttribute(attribute);
				indexmanager.createIndex(tmp3, indexname);
				break;
			}*/
			/*after creation,insert all the value already exists*/
			attrNode attr_property = getAttName(tablename, column);
			vector<attrNode>attr_list = catlog.getAttrList(tablename);
			vector<attrValues> all_tuples = recordmanager.select_attr(tablename,attr_list,column);
			vector<attrValues>::iterator tuples = all_tuples.begin();//get all the values in the attribute
			for (; tuples != all_tuples.end(); tuples++)
			{
				Target<float> tmp1(indexname);
				Target<int> tmp2(indexname);
				Target<string> tmp3(indexname);
				switch (attr_property.type)
				{
				case MINI_FLOAT:
					tmp1.setKey(tuples->value.f);
					tmp1.setIndex_info(tuples->ii);
					indexmanager.insert(tmp1);
					break;
				case MINI_INT:					
					tmp2.setKey(tuples->value.n);
					tmp2.setIndex_info(tuples->ii);
					indexmanager.insert(tmp2);
					break;
				case MINI_STRING:
					tmp3.setKey(tuples->value.s);
					tmp3.setIndex_info(tuples->ii);
					indexmanager.insert(tmp3);
					break;
				}
			}
			//may need this sentence
			delete &all_tuples;
			}
		catch (alreadyExist)
		{
		}
	}
	return 1;
}
int API::Insert(string tablename, vector<Union> data)
{
	/*insert the record into the recordmanager*/
	try{
		/*find the unique and primary value in index
		  if they already exist,abort,if not,change the property unique or primary to false.
		  If there is no index, send it to recordmanager to linear scan.
		*/
		vector<attrAndvalue> attlist;
		attrNode attr;
		for (int i = 0; i<data.size(); i++)
		{
			//transfer the attNode and Union to attrAndvalue
			attrAndvalue tmp;
			attr = getAttName(tablename, i);
			tmp.isPrimary = attr.isPrimary;
			tmp.isUnique = attr.isUnique;
			tmp.type = attr.type;
			tmp.length = attr.length;
			if (data[i].isNull) tmp.isNull = 1;
			else
			{
				switch (attr.type)
				{
				case MINI_INT:
					tmp.int_value = data[i].n;
					break;
				case MINI_FLOAT:
					tmp.float_value = data[i].f;
					break;
				case MINI_STRING:
					tmp.str_value = data[i].s;
					break;
				}
				tmp.isNull = 0;
			}
			string indexname = catlog.getIndexByAttrID(tablename, i);
			if (tmp.isPrimary&&indexname!=""||tmp.isUnique&&indexname!="")//test if already exists
			{	
				Target<string> tmp_str(indexname);
				Target<int> tmp_int(indexname);
				Target<string> tmp_float(indexname);
				vector<index_info> exist;
				switch (attr.type){
				case MINI_STRING:					
					tmp_str.setType = SINGLE;
					tmp_str.setKey = data[i].s;
					exist = indexmanager.search(tmp_str);
					if (!exist.empty()) { cout << "the "<<attr.attrName<<"can't have duplicate value!" << endl; return -1; }//already exists
					break;
				case MINI_INT:					
					tmp_int.setType = SINGLE;
					tmp_int.setKey = data[i].s;
					exist = indexmanager.search(tmp_int);
					if (!exist.empty()) { cout << "the " << attr.attrName << "can't have duplicate value!" << endl; return -1; }//already exists
					break;
				case MINI_FLOAT:					
					tmp_float.setType = SINGLE;
					tmp_float.setKey = data[i].s;
					exist = indexmanager.search(tmp_float);
					if (!exist.empty()) { cout << "the " << attr.attrName << "can't have duplicate value!" << endl; return -1; }//already exists
					break;
				}
				tmp.isPrimary = tmp.isUnique = 0;
			}
			attlist.push_back(tmp);
		}
		index_info tmp_index=recordmanager.insert(tablename, attlist);
		//insert the tuple into indexmanager
		vector<attrAndvalue>::iterator insert_it = attlist.begin();
		int i = 0;
		for (; insert_it != attlist.end(); insert_it++,i++)
		{
			string indexname = catlog.getIndexByAttrID(tablename, i++);
			if (indexname!="")//test if already exists
			{
				Target<string> tmp_str(indexname);
				Target<int> tmp_int(indexname);
				Target<string> tmp_float(indexname);
				switch (attr.type){
				case MINI_STRING:
					tmp_str.setType = SINGLE;
					tmp_str.setKey = data[i].s;
					tmp_str.setIndex_info(tmp_index);
					indexmanager.insert(tmp_str);
					break;
				case MINI_INT:
					tmp_int.setType = SINGLE;
					tmp_int.setKey = data[i].s;
					tmp_int.setIndex_info(tmp_index);
					indexmanager.insert(tmp_int);
					break;
				case MINI_FLOAT:
					tmp_float.setType = SINGLE;
					tmp_float.setKey = data[i].s;
					tmp_float.setIndex_info(tmp_index);
					indexmanager.insert(tmp_float);
					break;
				}
		}
			catlog.recordAdd(tablename, 1);
		return 1;//success
	}
	catch ()
	{
	}
}
int API::Delete(string tablename, vector<TreeNode> conditions)
{
	/*
	first find conditions with indexs.
	*/
	try{
		
		vector<TreeNode>::iterator	it = conditions.begin();
		vector<vector<index_info>> indexs;
		string indexname;
		for (; it != conditions.end(); it++)
		{
			if (it->op != 1){
				//if the operation is not inequal
				//find the index
				indexname = catlog.getIndexByAttrID(tablename, it->id);
				if (indexname != "")
				{
					//find all the index_info which fix in the condition
					attrNode attr = getAttName(tablename, it->id);
					Target<string> tmpStr(indexname);
					Target<float> tmpLF(indexname);
					Target<int> tmpInt(indexname);
					switch (it->op)//set the operation
					{
					case 0://:=
						tmpStr.setType(SINGLE);
						tmpLF.setType(SINGLE);
						tmpInt.setType(SINGLE);
						break;
					case 2://:<
						tmpStr.setType(LARGER);
						tmpLF.setType(LARGER);
						tmpInt.setType(LARGER);
						break;
					case 3://:>
						tmpStr.setType(LESS);
						tmpLF.setType(LESS);
						tmpInt.setType(LESS);
						break;
					case 4://:<=
						tmpStr.setType(NOTLARGER);
						tmpLF.setType(NOTLARGER);
						tmpInt.setType(NOTLARGER);
						break;
					case 5://:>=
						tmpStr.setType(NOTLESS);
						tmpLF.setType(NOTLESS);
						tmpInt.setType(NOTLESS);
						break;
					}
					switch (it->type)//search index_info from indexmanager
					{
					case MINI_FLOAT:
						tmpLF.setvalue(it->value.f);
						vector<index_info>& tt = indexmanager.search(tmpLF);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					case MINI_INT:
						tmpInt.setvalue(it->value.n);
						vector<index_info>& tt = indexmanager.search(tmpInt);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					case MINI_STRING:
						tmpStr.setvalue(it->value.s);
						vector<index_info>& tt = indexmanager.search(tmpStr);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					}
				}
			}
		}
		/*finishing finding the indexs in indexmanager*/
		//select from recordmanager
		vector<attrNode> attr = catlog.getAttrList(tablename);
		vector<index_info> merge_result;
		if (!indexs.empty()) merge_index(indexs, merge_result);
		/*
		delete the record using the recordmanager.
		*/
		int count = 0;
		vector<attrNode> attributes = catlog.getAttrList(tablename);
		vector<vector<deleted_node>>& deleted_attr = recordmanager.delete_index(tablename, conditions, attributes, merge_result);
		/*
		delete the index according to the recordmanager's return vector.
		*/
		vector<vector<deleted_node>>::iterator deleted_attr_it = deleted_attr.begin();
		for (; deleted_attr_it != deleted_attr.end(); deleted_attr_it++)
		{
			count = 0;
			vector<deleted_node>::iterator tuples = deleted_attr_it->begin();
			string indexname = catlog.getIndexByAttrID(tuples->att_id);
			if (indexname != ""){
				for (; tuples != deleted_attr_it->end(); tuples++)
				{
					switch (tuples->type)
					{
					case MINI_FLOAT:
						Target<float> tmp1(indexname);
						tmp1.setKey(tuples->value.f);
						tmp1.setType(SINGLE);
						indexmanager.delete(tmp1);
						break;
					case MINI_INT:
						Target<int> tmp2(indexname);
						tmp2.setKey(tuples->value.n);
						tmp2.setType(SINGLE);
						indexmanager.delete(tmp2);
						break;
					case MINI_STRING:
						Target<string> tmp3(indexname);
						tmp3.setKey(tuples->value.s);
						tmp3.setType(SINGLE);
						indexmanager.delete(tmp3);
						break;
					}
					count++;
				}
			}
		}
		catlog.recordDelete(tablename, count);
		return count;
	}
	catch ()
	{
	}
}
int API::dropTable(string tablename)
{
	try{
		//delete all the record

			/*delete all the indexs*/
			vector<string>Index = catlog.getIndexOfTable(tablename);
		vector<string>::iterator Indexs = Index.begin();
		for (; Indexs != Index.end(); Indexs++)
		{
			//delete all the indexs the table has
			indexmanager.deleteIndex(*Indexs);
			catlog.deleteIndex(*Indexs,tablename);
		}
		//delete the infomation in catalogManager
		catlog.deleteTable(tablename);
	}
	catch (/*****/){}
}
int API::dropIndex(string indexname,string tablename)
{
	try{
		/*delete index from indexmanager*/
		indexmanager.deleteIndex(indexname);
		/*delete index from catalog*/
		catlog.deleteIndex(indexname,tablename);
	}
	catch ()
	{

	}
}
int API::Select(string tablename, vector<TreeNode> conditions)
{
	try{
		vector<TreeNode>::iterator	it = conditions.begin();
		vector<vector<index_info>> indexs;
		string indexname;
		for (; it != conditions.end(); it++)
		{
			if (it->op != 1){
				//if the operation is not inequal
				//find the index
				indexname = catlog.getIndexByAttrID(tablename,it->id);
				if (indexname != "")
				{
					//find all the index_info which fix in the condition
					attrNode attr = getAttName(tablename, it->id);
					Target<string> tmpStr(tablename,indexname);
					Target<float> tmpLF(tablename,indexname);
					Target<int> tmpInt(tablename,indexname);
					switch (it->op)//set the operation
					{
					case 0://:=
						tmpStr.setType(SINGLE);
						tmpLF.setType(SINGLE);
						tmpInt.setType(SINGLE);
						break;
					case 2://:<
						tmpStr.setType(LARGER);
						tmpLF.setType(LARGER);
						tmpInt.setType(LARGER);
						break;
					case 3://:>
						tmpStr.setType(LESS);
						tmpLF.setType(LESS);
						tmpInt.setType(LESS);
						break;
					case 4://:<=
						tmpStr.setType(NOTLARGER);
						tmpLF.setType(NOTLARGER);
						tmpInt.setType(NOTLARGER);
						break;
					case 5://:>=
						tmpStr.setType(NOTLESS);
						tmpLF.setType(NOTLESS);
						tmpInt.setType(NOTLESS);						
						break;
					}
					switch (it->type)//search index_info from indexmanager
					{
					case MINI_FLOAT:
						tmpLF.setvalue(it->value.f);
						vector<index_info>& tt = indexmanager.searchSingle(tmpLF);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					case MINI_INT:
						tmpInt.setvalue(it->value.n);
						vector<index_info>& tt = indexmanager.searchSingle(tmpInt);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					case MINI_STRING:
						tmpStr.setvalue(it->value.s);
						vector<index_info>& tt = indexmanager.searchSingle(tmpStr);
						indexs.push_back(tt);
						delete &tt;
						it = conditions.erase(it);
						break;
					}
				}
			}
		}
		/*finishing finding the indexs in indexmanager*/
		//select from recordmanager
		vector<attrNode> attr = catlog.getAttrList(tablename);
		vector<index_info> merge_result;
		if (!indexs.empty())merge_index(indexs, merge_result);
		int select_number=recordmanager.select(tablename,conditions,attr, merge_result);
		//get the number selected
		return select_number;
	}
	catch ()
	{
	}
}
