// #include "stdafx.h"
#include"interface.h"
#include"catalogManager.h"
#include"API.h"
#include"string"
#include"iostream"
#include<algorithm>
#include<sys/time.h>
#include"recordManager.h"
using namespace std;
class ERROR{};
bool compare(index_info a, index_info b)
{
    if (a.num < b.num)return true;
    else if (a.num == b.num) return a.offset < b.offset;
    return false;
}
void merge_index(vector<vector<index_info> >& index, vector<index_info>& result)
{
    //tested
    vector<vector<index_info> >::iterator it = index.begin();
    for (; it != index.end(); it++)
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
attrNode API::getAttName(string tablename, int column)
{
    list<attrNode>temp = catlog.getAttrList(tablename);
    list<attrNode>::iterator it = temp.begin();
    for (int i = 0; i < column; i++)
    {
        it++;
    }
    return *it;
}
int insert_count = 0;
int API::exec(Interface inter)
{
    timeval start, end, Null;
    gettimeofday(&start, &Null);
    //cout << "enterint API!!!!" << endl;
    int op = inter.whichOperation();
    string tablename = inter.getTableName();
    vector<attrNode> tableAttr;
    vector<Union> data = inter.getData();
    int type = inter.whichObject();
    int count;
    switch (op){
        case MINI_CREATE://create index or table
            
            switch (type)
        {
            case MINI_TABLE:
                switch (createTable(tablename, inter.getDefinitions()))
            {
                case 1:cout << "creation success!" << endl; break;
                case 0:cout << "Already Exists!" << endl; break;
                case -1:cout << "create failure" << endl; break;
            }
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
                catch (ERROR){}
                break;
        }
            break;
        case MINI_DROP://drop table and index
            
            switch (inter.whichObject())
        {
            case MINI_TABLE:
                switch (dropTable(tablename))
            {
                case 1:cout << "success!" << endl; break;
                default:cout << "ERROR" << endl;
            }
                break;
            case MINI_INDEX:
                /*delete the index */
                switch (dropIndex(inter.getIndexName(), inter.getTableName()))
            {
                case 1:cout << "success!" << endl; break;
                default: cout << "ERROR" << endl;
            }
                break;
        }
            break;
        case MINI_SELECT:
            //edit needed
            count = Select(tablename, inter.getConditions());
            cout << "The amount selected:" << count << endl;
            break;
        case MINI_INSERT:
            /*make a vector of attrAndvalue*/
        {
            int result = Insert(tablename, data);
            switch (result){
                case -1:cout << "abort because of duplication!" << endl;
                    break;
               // case 1:cout << "insert successfully!" <<insert_count<< endl;
                    insert_count++;
            }
            break;
        }
        case MINI_DELETE:
            int count = Delete(tablename, inter.getConditions());
            switch (count){
                case -2:cout << "No tuples deleted!" << endl; break;
                default:	cout << "The amount deleted:" << count << endl;
            }
            break;
    }
    //indexmanager.flushAll();
    //record_manager_.flushAll();
    gettimeofday(&end, &Null);
    double t = 1000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
   // if( t < 1000 )
     //   cout << "Query finished in " << t << " ms" << endl;
    //else
      //  cout << "Query finished in " << t/1000 << " s" << endl;
    return 0;
}
template<class T>
list<T> VectorToList(vector<T> a)
{
    typename vector<T>::iterator it = a.begin();
    list<T> b;
    for (; it != a.end(); it++)
    {
        b.push_back(*it);
    }
    return b;
}
template<class T>
vector<T> ListToVector(list<T> a)
{
    typename list<T>::iterator it = a.begin();
    vector<T> b;
    for (; it != a.end(); it++)
    {
        b.push_back(*it);
    }
    return b;
}
int API::createTable(string tablename, vector<attrNode> attributes)  //throw(alreadyExist,CreationFailureOnCatalog,)
{
    //return value 0 -1 1
    //0 means existed,-1 creation failure, 1 success
    cout << "entering create table!" << endl;
    int flag = catlog.doesTableExist(tablename);
    cout << "table flag " << flag << endl;
    if (flag)
        return 0;//existed
    else
    {
        try
        {
            int offset = 0;
            vector<attrNode>::iterator att_it = attributes.begin();
            for (; att_it != attributes.end(); att_it++)
            {
                att_it->offset = offset;
                switch (att_it->type)
                {
                    case MINI_FLOAT:
                        offset += 4;
                        att_it->length = 4;
                        break;
                    case MINI_INT:
                        offset += 4;
                        att_it->length = 4;
                        break;
                    case MINI_STRING:
                        offset += att_it->length;
                        break;
                }
            }
            cout << "entering create table!" << endl;
            if (catlog.createTable(tablename, VectorToList<attrNode>(attributes)))
            {
                cout << "creating indexs!" << endl;
                vector<attrNode>::iterator it = attributes.begin();
                int column = 0;
                for (; it != attributes.end(); it++)
                {
                    if (it->isPrimary)
                    {
                        //catlog.createIndex(tablename + ' ' + it->attrName, tablename, column);
                        cout << "now creating index" << endl;
                        return (createIndex(tablename, it->attrName, it->attrName, column));
                    }
                    column++;
                }
                return -2;
            }
            else {
                cout << "createTable in catalog failure" << endl;
                return -1;//creation failure
            }
        }
        catch (ERROR)
        {
            return -1;
        }
    }
}
int API::createIndex(string tablename, string indexname, string attribute, int column)//throw(alreadyExist,CreationFailureOnBplus)
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
            attrNode attrPro = getAttName(tablename,column);
            
            Target<float> tmp1(indexname);
            Target<int> tmp2(indexname);
            Target<string> tmp3(indexname);
            switch (attrPro.type)
            {//for different type , create a corressponding index.
                case MINI_FLOAT:
                    indexmanager.createIndex(tmp1);
                    break;
                case MINI_INT:
                    indexmanager.createIndex(tmp2);
                    break;
                case MINI_STRING:
                    indexmanager.createIndex(tmp3);
                    break;
            }
            /*after creation,insert all the value already exists*/
            attrNode attr_property = getAttName(tablename, column);
            vector<attrNode>attr_list = ListToVector(catlog.getAttrList(tablename));
            cout << "selecting from recordManager!" << endl;
            vector<attrValue> all_tuples = record_manager_.select_attr(tablename, attr_list, column);
            cout << "selecting finish" << endl;
            vector<attrValue>::iterator tuples = all_tuples.begin();//get all the values in the attribute
            cout << "inserting to index" << endl;
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
            
            cout << "create index successfully" << endl;
        }
        catch (ERROR)
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
        for (int i = 0; i < data.size(); i++)
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
            if (tmp.isPrimary&&indexname != "" || tmp.isUnique&&indexname != "")//test if already exists
            {
                Target<string> tmp_str(indexname);
                Target<int> tmp_int(indexname);
                Target<float> tmp_float(indexname);
                vector<index_info> exist;
                switch (attr.type){
                    case MINI_STRING:
                        tmp_str.setType(SINGLE);
                        tmp_str.setKey(data[i].s);
                        exist = indexmanager.search(tmp_str);
                        if (!exist.empty()) { cout << "the " << attr.attrName << " can't have duplicate value!" << endl; return -1; }//already exists
                        break;
                    case MINI_INT:
                        tmp_int.setType(SINGLE);
                        tmp_int.setKey(data[i].n);
                        exist = indexmanager.search(tmp_int);
                        if (!exist.empty()) { cout << "the " << attr.attrName << "can't have duplicate value!" << endl; return -1; }//already exists
                        break;
                    case MINI_FLOAT:
                        tmp_float.setType(SINGLE);
                        tmp_float.setKey(data[i].f);
                        exist = indexmanager.search(tmp_float);
                        if (!exist.empty()) { cout << "the " << attr.attrName << "can't have duplicate value!" << endl; return -1; }//already exists
                        break;
                }
                tmp.isPrimary = tmp.isUnique = 0;
            }
            else
            {
                if (tmp.isUnique&&indexname == "")
                {
                    vector<attrNode> tttmp = ListToVector(catlog.getAttrList(tablename));
                    vector<attrValue> all_tuples = record_manager_.select_attr(tablename, tttmp, i);
                    vector<attrValue>::iterator tuple = all_tuples.begin();
                    for (; tuple != all_tuples.end(); tuple++)
                    {
                        switch (attr.type)
                        {
                            case MINI_INT:
                                if (data[i].n == tuple->value.n) { cout << "the " << attr.attrName << "can't have duplicate value!"; return -1; }
                                break;
                            case MINI_FLOAT:
                                if (data[i].f == tuple->value.f){ cout << "the " << attr.attrName << "can't have duplicate value!"; return -1; }
                                break;
                            case MINI_STRING:
                                if (data[i].s == tuple->value.s){ cout << "the " << attr.attrName << "can't have duplicate value!"; return -1; }
                                break;
                        }
                    }
                    tmp.isUnique = 0;
                }
            }
            attlist.push_back(tmp);
        }
        vector<attrNode> zkw = ListToVector<attrNode>(catlog.getAttrList(tablename));
        index_info tmp_index = record_manager_.insert(tablename, attlist, zkw);
        //insert the tuple into indexmanager
        vector<attrAndvalue>::iterator insert_it = attlist.begin();
        int i = 0;
        for (; insert_it != attlist.end(); insert_it++, i++)
        {
            string indexname = catlog.getIndexByAttrID(tablename, i);
            if (indexname != "")//test if already exists
            {
                Target<string> tmp_str(indexname);
                Target<int> tmp_int(indexname);
                Target<float> tmp_float(indexname);
                switch (insert_it->type){
                    case MINI_STRING:
                        tmp_str.setType(SINGLE);
                        tmp_str.setKey(data[i].s);
                        tmp_str.setIndex_info(tmp_index);
                        indexmanager.insert(tmp_str);
                        break;
                    case MINI_INT:
                        tmp_int.setType(SINGLE);
                        tmp_int.setKey(data[i].n);
                        tmp_int.setIndex_info(tmp_index);
                        indexmanager.insert(tmp_int);
                        break;
                    case MINI_FLOAT:
                        tmp_float.setType(SINGLE);
                        tmp_float.setKey(data[i].f);
                        tmp_float.setIndex_info(tmp_index);
                        indexmanager.insert(tmp_float);
                        break;
                }
            }
            catlog.recordAdd(tablename, 1);
        }
        return 1;
    }
    catch (ERROR)
    {
        return -1;
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
        int flag = 0;
        for (; it != conditions.end();)
        {
            if (it->op != NE){
                //if the operation is not inequal
                //find the index
                
                indexname = catlog.getIndexByAttrID(tablename, it->id);
                if (indexname != "")
                {
                    flag = 1;
                    //find all the index_info which fix in the condition
                    attrNode attr = getAttName(tablename, it->id);
                    Target<string> tmpStr(indexname);
                    Target<float> tmpLF(indexname);
                    Target<int> tmpInt(indexname);
                    switch (it->op)//set the operation
                    {
                        case EQU://:=
                            tmpStr.setType(SINGLE);
                            tmpLF.setType(SINGLE);
                            tmpInt.setType(SINGLE);
                            break;
                        case GT://:>
                            tmpStr.setType(LARGER);
                            tmpLF.setType(LARGER);
                            tmpInt.setType(LARGER);
                            break;
                        case LT://:<
                            tmpStr.setType(LESS);
                            tmpLF.setType(LESS);
                            tmpInt.setType(LESS);
                            break;
                        case LE://:<=
                            tmpStr.setType(NOTLARGER);
                            tmpLF.setType(NOTLARGER);
                            tmpInt.setType(NOTLARGER);
                            break;
                        case GE://:>=
                            tmpStr.setType(NOTLESS);
                            tmpLF.setType(NOTLESS);
                            tmpInt.setType(NOTLESS);
                            break;
                    }
                    vector<index_info> tt;
                    switch (it->type)//search index_info from indexmanager
                    {
                        case MINI_FLOAT:
                            tmpLF.setKey(it->value.f);
                            tt = indexmanager.search(tmpLF);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                        case MINI_INT:
                            tmpInt.setKey(it->value.n);
                            tt = indexmanager.search(tmpInt);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                        case MINI_STRING:
                            tmpStr.setKey(it->value.s);
                            tt = indexmanager.search(tmpStr);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                    }
                }
                else it++;
            } else
                if (it != conditions.end()) it++;
        }
        /*finishing finding the indexs in indexmanager*/
        //select from recordmanager
        vector<attrNode> attr = ListToVector(catlog.getAttrList(tablename));
        vector<index_info> merge_result;
        if (!indexs.empty()) merge_index(indexs, merge_result);
        if (flag && merge_result.empty()) return -2;
        /*
         delete the record using the recordmanager.
         */
        int count = 0;
        vector<attrNode> attributes = ListToVector(catlog.getAttrList(tablename));
        vector<vector<deleted_node>> deleted_attr = record_manager_.delete_tuple(tablename, conditions, attributes, merge_result);
        /*
         delete the index according to the recordmanager's return vector.
         */
        vector<vector<deleted_node>>::iterator deleted_attr_it = deleted_attr.begin();
        for (; deleted_attr_it != deleted_attr.end(); deleted_attr_it++)
        {
            count = 0;
            if (deleted_attr_it->size()==0) continue;
            vector<deleted_node>::iterator tuples = deleted_attr_it->begin();
            string indexname = catlog.getIndexByAttrID(tablename, tuples->att_id);
            if (indexname != ""){
                for (; tuples != deleted_attr_it->end(); tuples++)
                {
                    Target<float> tmp1(indexname);
                    Target<int> tmp2(indexname);
                    Target<string> tmp3(indexname);
                    switch (tuples->type)
                    {
                        case MINI_FLOAT:
                            tmp1.setKey(tuples->value.f);
                            tmp1.setType(SINGLE);
                            indexmanager.Delete(tmp1);
                            break;
                        case MINI_INT:
                            tmp2.setKey(tuples->value.n);
                            tmp2.setType(SINGLE);
                            indexmanager.Delete(tmp2);
                            break;
                        case MINI_STRING:
                            tmp3.setKey(tuples->value.s);
                            tmp3.setType(SINGLE);
                            indexmanager.Delete(tmp3);
                            break;
                    }
                    count++;
                }
            }
        }
        catlog.recordDelete(tablename, count);
        return count;
    }
    catch (ERROR)
    {
        return -1;
    }
}
int API::dropTable(string tablename)
{
    try{
        //delete all the record
        
        /*delete all the indexs*/
        /*vector<string>Index = catlog.getIndexOfTable(tablename);
         vector<string>::iterator Indexs = Index.begin();
         for (; Indexs != Index.end(); Indexs++)
         {
         //delete all the indexs the table has
         indexmanager.deleteIndex(*Indexs);
         catlog.deleteIndex(*Indexs,tablename);
         }*/
        //delete the infomation in catalogManager
        cout << "dorping table" << endl;
        list<string> tmp = catlog.getIndexOfTable(tablename);
        list<string>::iterator it = tmp.begin();
        for (; it != tmp.end(); it++)
        {
            Target<int> tt(*it);
            indexmanager.drop(tt);
        }
        return catlog.deleteTable(tablename);
        
    }
    catch (ERROR){ return -1; }
}
int API::dropIndex(string indexname, string tablename)
{
    try{
        /*delete index from indexmanager*/
        //indexmanager.deleteIndex(indexname);
        /*delete index from catalog*/
        string name = catlog.getIndexFileByIndexName(indexname);
        Target<int> a(name);
        indexmanager.drop(a);
        return catlog.deleteIndex(indexname);
    }
    catch (ERROR)
    {
        return -1;
    }
}
int API::Select(string tablename, vector<TreeNode> conditions)
{
    try{
        vector<TreeNode>::iterator	it = conditions.begin();
        vector<vector<index_info>> indexs;
        string indexname;
        int flag = 0;
        for (; it != conditions.end();)
        {
            
            if (it->op != NE){
                //if the operation is not inequal
                //find the index
                indexname = catlog.getIndexByAttrID(tablename, it->id);
                if (indexname != "")
                {
                    //find all the index_info which fix in the condition
                    flag = 1;
                    attrNode attr = getAttName(tablename, it->id);
                    Target<string> tmpStr(indexname);
                    Target<float> tmpLF(indexname);
                    Target<int> tmpInt(indexname);
                    switch (it->op)//set the operation
                    {
                        case EQU://:=
                            tmpStr.setType(SINGLE);
                            tmpLF.setType(SINGLE);
                            tmpInt.setType(SINGLE);
                            break;
                        case LT://:<
                            tmpStr.setType(LESS);
                            tmpLF.setType(LESS);
                            tmpInt.setType(LESS);
                            break;
                        case GT://:>
                            tmpStr.setType(LARGER);
                            tmpLF.setType(LARGER);
                            tmpInt.setType(LARGER);
                            break;
                        case LE://:<=
                            tmpStr.setType(NOTLARGER);
                            tmpLF.setType(NOTLARGER);
                            tmpInt.setType(NOTLARGER);
                            break;
                        case GE://:>=
                            tmpStr.setType(NOTLESS);
                            tmpLF.setType(NOTLESS);
                            tmpInt.setType(NOTLESS);
                            break;
                    }
                    vector<index_info> tt;
                    switch (it->type)//search index_info from indexmanager
                    {
                        case MINI_FLOAT:
                            tmpLF.setKey(it->value.f);
                            tt = indexmanager.search(tmpLF);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                        case MINI_INT:
                            tmpInt.setKey(it->value.n);
                            tt = indexmanager.search(tmpInt);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                        case MINI_STRING:
                            tmpStr.setKey(it->value.s);
                            tt = indexmanager.search(tmpStr);
                            indexs.push_back(tt);
                            it = conditions.erase(it);
                            break;
                    }
                }
                else it++;
            }
            else
                if (it != conditions.end()) it++;
        }
        /*finishing finding the indexs in indexmanager*/
        //select from recordmanager
        vector<attrNode> attr = ListToVector(catlog.getAttrList(tablename));
        vector<index_info> merge_result;
        if (!indexs.empty())merge_index(indexs, merge_result);
        if (flag && merge_result.empty())
        {cout << "select: 0 rows affect(s)" << endl;return 0;}
        else
            record_manager_.select(tablename, conditions, attr, merge_result);
        //get the number selected
        return record_manager_.getCount();
    }
    catch (ERROR)
    {
        return -1;
    }
}
