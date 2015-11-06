#include "stdafx.h"
#include "recordManager.h"

recordManager::recordManager(){}
recordManager::~recordManager(){}



<<<<<<< HEAD
void recordManager::judge(bool &notMatch, vector <TreeNode> &v_tn, vector <attrNode> &v_an, bufferNode *p_temp_bn, int &offset) {

	vector <TreeNode>::iterator it_tn;
	for (it_tn = v_tn.begin(); it_tn != v_tn.end(); it_tn++) {
		TreeNode temp_tn = *it_tn;
		attrNode temp_an = v_an[temp_tn.id];
		int isNull;
		p_temp_bn->readBlock(isNull, 4, offset + 4 + 4 * temp_tn.id + temp_an.offset);
		if (isNull){
			switch (temp_tn.op){
			case 0:
				notMatch = !(isNull == 1);	break;
			case 1:
				notMatch = !(isNull == 0);	break;
			default:
				break;
			}
		}
		else{
			switch (temp_tn.type){
			case 1:
				int temp_int;
				p_temp_bn->readBlock(temp_int, 4, offset + 4 + 4 * (temp_tn.id + 1) + temp_an.offset);
				switch (temp_tn.op){
				case 0:
					notMatch = !(temp_int == temp_tn.value.n);
					break;
				case 1:
					notMatch = !(temp_int != temp_tn.value.n);
					break;
				case 2:
					notMatch = !(temp_int < temp_tn.value.n);
					break;
				case 3:
					notMatch = !(temp_int > temp_tn.value.n);
					break;
				case 4:
					notMatch = !(temp_int <= temp_tn.value.n);
					break;
				case 5:
					notMatch = !(temp_int >= temp_tn.value.n);
					break;
				default:
					break;
=======
void recordManager::judge(bool &notMatch, vector <TreeNode> &v_tn , vector <attrNode> &v_an, bufferNode *p_temp_bn, int &offset) {

	vector <TreeNode>::iterator it_tn;
	for(it_tn = v_tn.begin(); it_tn != v_tn.end(); it_tn++) {
					TreeNode temp_tn = *it_tn;
					attrNode temp_an = v_an[temp_tn.id];
					int isNull;
					p_temp_bn->readBlock(isNull,4,offset + 4 + 4*temp_tn.id + temp_an.offset);
					if (isNull){
						switch(temp_tn.op){
							case 0:	
								notMatch = !(isNull == 1);	break;
							case 1:
								notMatch = !(isNull == 0);	break;
							default:
								break;
						}
					}
					else{
						switch(temp_tn.type){
							case 1:
								int temp_int ;
								p_temp_bn->readBlock(temp_int, 4 , offset + 4 + 4*(temp_tn.id+1) + temp_an.offset);
								switch(temp_tn.op){
									case 0:
										notMatch = !(temp_int == temp_tn.value.n);
										break;
									case 1:
										notMatch = !(temp_int != temp_tn.value.n);
										break; 
									case 2:
										notMatch = !(temp_int < temp_tn.value.n);
										break;
									case 3:
										notMatch = !(temp_int > temp_tn.value.n);
										break;
									case 4:
										notMatch = !(temp_int <= temp_tn.value.n);
										break;
									case 5:
										notMatch = !(temp_int >= temp_tn.value.n);
										break;
									default:
										break;
								}
								break;
							case 2:
							{
								string temp_str;
								p_temp_bn->readBlock(temp_str , temp_an.length , offset + 4 + 4*(temp_tn.id+1) + temp_an.offset);
								switch(temp_tn.op){
									case 0:
										notMatch = !(temp_str == temp_tn.value.s);
										break;
									case 1:
										notMatch = !(temp_str != temp_tn.value.s);
										break; 
									case 2:
										notMatch = !(temp_str < temp_tn.value.s);
										break;
									case 3:
										notMatch = !(temp_str > temp_tn.value.s);
										break;
									case 4:
										notMatch = !(temp_str <= temp_tn.value.s);
										break;
									case 5:
										notMatch = !(temp_str >= temp_tn.value.s);
										break;
									default:
										break;
								}
								break;
							}
								
							case 3:
								float temp_f;
								p_temp_bn->readBlock(temp_f, 4, offset + 4 + 4*(temp_tn.id+1) + temp_an.offset);
								switch(temp_tn.op){
									case 0:
										notMatch = !(temp_f == temp_tn.value.f);
										break;
									case 1:
										notMatch = !(temp_f != temp_tn.value.f);
										break; 
									case 2:
										notMatch = !(temp_f < temp_tn.value.f);
										break;
									case 3:
										notMatch = !(temp_f > temp_tn.value.f);
										break;
									case 4:
										notMatch = !(temp_f <= temp_tn.value.f);
										break;
									case 5:
										notMatch = !(temp_f >= temp_tn.value.f);
										break;
									default:
										break;
								}
								break;	
							default:
								break;		
						}
					}
					if(notMatch)
						break;	
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
				}
				break;
			case 2:
			{
					  string temp_str;
					  p_temp_bn->readBlock(temp_str, temp_an.length, offset + 4 + 4 * (temp_tn.id + 1) + temp_an.offset);
					  switch (temp_tn.op){
					  case 0:
						  notMatch = !(temp_str == temp_tn.value.s);
						  break;
					  case 1:
						  notMatch = !(temp_str != temp_tn.value.s);
						  break;
					  case 2:
						  notMatch = !(temp_str < temp_tn.value.s);
						  break;
					  case 3:
						  notMatch = !(temp_str > temp_tn.value.s);
						  break;
					  case 4:
						  notMatch = !(temp_str <= temp_tn.value.s);
						  break;
					  case 5:
						  notMatch = !(temp_str >= temp_tn.value.s);
						  break;
					  default:
						  break;
					  }
					  break;
			}

			case 3:
				float temp_f;
				p_temp_bn->readBlock(temp_f, 4, offset + 4 + 4 * (temp_tn.id + 1) + temp_an.offset);
				switch (temp_tn.op){
				case 0:
					notMatch = !(temp_f == temp_tn.value.f);
					break;
				case 1:
					notMatch = !(temp_f != temp_tn.value.f);
					break;
				case 2:
					notMatch = !(temp_f < temp_tn.value.f);
					break;
				case 3:
					notMatch = !(temp_f > temp_tn.value.f);
					break;
				case 4:
					notMatch = !(temp_f <= temp_tn.value.f);
					break;
				case 5:
					notMatch = !(temp_f >= temp_tn.value.f);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		if (notMatch)
			break;
	}
	return;
}

<<<<<<< HEAD
void recordManager::print(vector <attrNode> &v_an, bufferNode *p_temp_bn, int offset) {
=======
void recordManager::print(vector <attrNode> &v_an , bufferNode *p_temp_bn, int offset) {
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9

	vector <attrNode>::iterator it_an;

	count++;
	cout << count << '\t';
	int counter = 0;
	for (it_an = v_an.begin(); it_an != v_an.end(); it_an++, counter++) {
		int isNull;
		attrNode temp_an = *it_an;
<<<<<<< HEAD
		p_temp_bn->readBlock(isNull, 4, offset + 4 + 4 * counter + temp_an.offset);
		if (isNull){
			cout << "null\t";
		}
		else{
			switch (temp_an.type) {
			case 1:
				int t_int;
				p_temp_bn->readBlock(t_int, 4, offset + 4 + 4 * (counter + 1) + temp_an.offset);
				cout << t_int << '\t';
				break;
			case 2:
			{
					  string t_str;
					  p_temp_bn->readBlock(t_str, temp_an.length, offset + 4 + 4 * (counter + 1) + temp_an.offset);
					  cout << t_str << '\t';
					  break;
			}
			case 3:
				float t_f;
				p_temp_bn->readBlock(t_f, 4, offset + 4 + 4 * (counter + 1) + temp_an.offset);
				cout << t_f << '\t';
				break;
			default:
				cout << "shenmegui ?";
				break;
=======
		p_temp_bn->readBlock(isNull,4,offset + 4 + 4*counter + temp_an.offset);
		if(isNull){
			cout << "null\t" ;
		}
		else{
			switch(temp_an.type) {
				case 1:
					int t_int;
					p_temp_bn->readBlock(t_int,4,offset + 4 + 4*(counter+1) + temp_an.offset);
					cout << t_int << '\t';
					break;
				case 2:
				{
					string t_str;
					p_temp_bn->readBlock(t_str,temp_an.length,offset + 4 + 4*(counter+1) + temp_an.offset);
					cout << t_str << '\t';
					break;
				}
				case 3:
					float t_f;
					p_temp_bn->readBlock(t_f,4,offset + 4 + 4*(counter+1) + temp_an.offset);
					cout << t_f << '\t';
					break;
				default:
					cout << "shenmegui ?";
					break;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
			}
		}
	}
	cout << endl;
	return;
}

<<<<<<< HEAD
void recordManager::delAndGetValue(vector < vector < deleted_node > > &del_ind, vector<attrNode> &v_an, vector<int> &v_indexNum, map<int, int> &m, bufferNode *p_temp_bn, int &offset) {
	vector <int>::iterator it_in;
	count++;
	p_temp_bn->writeBlock((int)1, 4, offset);
	for (it_in = v_indexNum.begin(); it_in != v_indexNum.end(); it_in++){
=======
void recordManager::delAndGetValue(vector < vector < deleted_node > > &del_ind, vector<attrNode> &v_an, vector<int> &v_indexNum, map<int,int> &m, bufferNode *p_temp_bn, int &offset) {
	vector <int>::iterator it_in;
	count++;
	p_temp_bn->writeBlock((int)1,4,offset);
	for(it_in = v_indexNum.begin();it_in != v_indexNum.end();it_in++){
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
		int t_in = *it_in;
		attrNode temp_an = v_an[t_in];
		deleted_node del_node(temp_an.type, t_in);
		int isNull;
<<<<<<< HEAD
		p_temp_bn->readBlock(isNull, 4, offset + 4 + 4 * t_in + temp_an.offset);
		if (isNull){
			del_node.value.isNull = true;
		}
		else{
			switch (temp_an.type){
			case 1:
				int temp_int;
				p_temp_bn->readBlock(temp_int, 4, offset + 4 + 4 * (t_in + 1) + temp_an.offset);
				del_node.value.n = temp_int;
				break;
			case 2:
			{
					  string temp_str;
					  p_temp_bn->readBlock(temp_str, temp_an.length, offset + 4 + 4 * (t_in + 1) + temp_an.offset);
					  del_node.value.s = temp_str;
					  break;
			}

			case 3:
				float temp_f;
				p_temp_bn->readBlock(temp_f, 4, offset + 4 + 4 * (t_in + 1) + temp_an.offset);
				del_node.value.f = temp_f;
				break;
			default:
				break;
=======
		p_temp_bn->readBlock(isNull,4,offset+4+4*t_in+temp_an.offset);
		if(isNull){
			del_node.value.isNull = true;
		}
		else{
			switch(temp_an.type){
				case 1:
					int temp_int;
					p_temp_bn->readBlock(temp_int,4,offset+4+4*(t_in+1)+temp_an.offset);
					del_node.value.n = temp_int;
					break;
				case 2:
				{
					string temp_str;
					p_temp_bn->readBlock(temp_str,temp_an.length,offset+4+4*(t_in+1)+temp_an.offset);
					del_node.value.s = temp_str;
					break;
				}
					
				case 3:
					float temp_f;
					p_temp_bn->readBlock(temp_f,4,offset+4+4*(t_in+1)+temp_an.offset);
					del_node.value.f = temp_f;
					break;
				default:
					break;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
			}
		}
		del_ind[m[t_in]].push_back(del_node);
	}
}

int recordManager::select(string table_name, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info>  &join_ii) {

	vector<TreeNode>::iterator it_tn;
	vector<index_info>::iterator it_ii;
	vector<attrNode>::iterator it_an;

<<<<<<< HEAD
	int r_length = 4 + 4 * v_an.size() + v_an.back().offset + v_an.back().length;
	//r_length += 4 + 4*v_an.size();
	for (it_an = v_an.begin(); it_an != v_an.end(); it_an++){
		cout << '\t' << it_an->attrName;
	}
	cout << endl;
	count = 0;

	if (!join_ii.empty()) {
		//intersect(vector<index_info> &join_ii,vector < vector <index_info> > &join_ii);
		index_info temp_ii = { -1, -1 }; //�ж��Ƿ���ͬһ��block�ڣ�
		bufferNode *p_temp_bn = getBlockPointer(0, table_name, join_ii[0].num);
		for (it_ii = join_ii.begin(); it_ii != join_ii.end(); it_ii++) {
			if (it_ii->num != temp_ii.num){
				temp_ii = *it_ii;
				p_temp_bn = getBlockPointer(0, table_name, temp_ii.num);
			}
			temp_ii.offset = it_ii->offset;
			int delOrUnwrite;
			p_temp_bn->readBlock(delOrUnwrite, 4, temp_ii.offset);
			if (delOrUnwrite)
				continue;
			if (!v_tn.empty()){
				bool notMatch = false;
				judge(notMatch, v_tn, v_an, p_temp_bn, temp_ii.offset);
				if (notMatch)
					continue;
			}

			print(v_an, p_temp_bn, temp_ii.offset);
=======
	int r_length = 4 + 4*v_an.size() + v_an.back().offset + v_an.back().length;
	//r_length += 4 + 4*v_an.size();
	for (it_an = v_an.begin(); it_an !=v_an.end(); it_an++){
		cout << '\t' << it_an->attrName ;
	}
	cout << endl;
	count = 0;
	
	if (!join_ii.empty()) {
		//intersect(vector<index_info> &join_ii,vector < vector <index_info> > &join_ii);
		index_info temp_ii = {-1,-1}; //判断是否在同一个block内；
		bufferNode *p_temp_bn = getBlockPointer(0, table_name, join_ii[0].num);
		for (it_ii = join_ii.begin(); it_ii != join_ii.end(); it_ii++) {
			if(it_ii->num != temp_ii.num){
				temp_ii = *it_ii;
				p_temp_bn = getBlockPointer(0,table_name,temp_ii.num);
			}
			temp_ii.offset = it_ii->offset;
			int delOrUnwrite ;
			p_temp_bn->readBlock(delOrUnwrite, 4, temp_ii.offset);
			if(delOrUnwrite)
				continue;
			if(!v_tn.empty()){
				bool notMatch = false;
				judge(notMatch,v_tn,v_an,p_temp_bn,temp_ii.offset);
				if(notMatch)
					continue;
			}
			
			print(v_an,p_temp_bn,temp_ii.offset);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
		}
		cout << endl << ' ' << count << " record(s) are affected. " << endl;
		return 0;
	}
	else {
		bool isEnd = false;
		int b_counter = 0;
<<<<<<< HEAD
		while (!isEnd){
			bufferNode *p_temp_bn = getBlockPointer(0, table_name, b_counter++);
			int current_offset = 0;
			while (current_offset + r_length < 4096) {
				int delOrUnwrite = 0;
				p_temp_bn->readBlock(delOrUnwrite, 4, current_offset);
				if (delOrUnwrite == 1){
					current_offset += r_length;
					continue;
				}
				if (delOrUnwrite == 2){
					isEnd = true;
					break;
				}
				if (!v_tn.empty()){
					bool notMatch = false;
					judge(notMatch, v_tn, v_an, p_temp_bn, current_offset);
					if (notMatch){
						current_offset += r_length;
						continue;
					}
				}

				print(v_an, p_temp_bn, current_offset);
				current_offset += r_length;
			}
=======
		while(!isEnd){
			bufferNode *p_temp_bn = getBlockPointer(0,table_name,b_counter++);
			int current_offset = 0;
			while (current_offset + r_length < 4096) {
				int delOrUnwrite = 0;
				p_temp_bn->readBlock(delOrUnwrite,4,current_offset);
				if(delOrUnwrite==1){
					current_offset+=r_length;
					continue;
				}
				if(delOrUnwrite==2){
					isEnd = true;
					break;
				}
				if(!v_tn.empty()){
					bool notMatch = false;
					judge(notMatch,v_tn,v_an,p_temp_bn,current_offset);
					if (notMatch){
						current_offset += r_length;
						continue;
					}
				}
				
				print(v_an,p_temp_bn,current_offset);
				current_offset += r_length;
			}		
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
		}
		cout << endl << ' ' << count << " record(s) are affected. " << endl;
		return 0;
	}
}

index_info& recordManager::insert(string table_name, vector <attrAndvalue> &v_aav){

	vector<attrAndvalue>::iterator it_aav;
	int r_length = 4;
	for (it_aav = v_aav.begin(); it_aav != v_aav.end(); it_aav++){
		r_length += (4 + it_aav->length);
	}
	//int r_length = 4 + 4*v_aav.size() + v_aav.back().offset + v_aav.back().length;
	//r_length += 4 + 4*v_aav.size();
	bool isEnd = false;
	int b_counter = 0;
<<<<<<< HEAD
	while (!isEnd){
		bufferNode &temp_bn = getBlock(0, table_name, b_counter++);
=======
	while(!isEnd){
		bufferNode &temp_bn = getBlock(0,table_name,b_counter++);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
		int current_offset = 0;
		while (current_offset + r_length < 4096){
			int delOrUnwrite;
			temp_bn.readBlock(delOrUnwrite, 4, current_offset);
			if (delOrUnwrite){
				int inner_offset = 4;
				temp_bn.writeBlock((int)0, 4, current_offset);
				for (it_aav = v_aav.begin(); it_aav != v_aav.end(); it_aav++){
					attrAndvalue temp_aav = *it_aav;
					switch (temp_aav.type){
					case 1:
						if (temp_aav.isNull){
							temp_bn.writeBlock((int)1, 4, current_offset + inner_offset);
							temp_bn.writeBlock((int)0, 4, current_offset + inner_offset + 4);
						}
						else{
							temp_bn.writeBlock((int)0, 4, current_offset + inner_offset);
							temp_bn.writeBlock(temp_aav.int_value, 4, current_offset + inner_offset + 4);
						}
						inner_offset += 8;	break;
					case 2:
					{
							  string temp_str = "null";
							  if (temp_aav.isNull){
								  temp_bn.writeBlock((int)1, 4, current_offset + inner_offset);
								  temp_bn.writeBlock(temp_str, 4, current_offset + inner_offset + 4);
							  }
							  else{
								  temp_bn.writeBlock((int)0, 4, current_offset + inner_offset);
								  temp_bn.writeBlock(temp_aav.str_value, temp_aav.length, current_offset + inner_offset + 4);
							  }
							  inner_offset += 4 + temp_aav.length;
							  break;
					}

					case 3:
						if (temp_aav.isNull){
							temp_bn.writeBlock((int)1, 4, current_offset + inner_offset);
							temp_bn.writeBlock((float)0, 4, current_offset + inner_offset + 4);
						}
						else{
							temp_bn.writeBlock((int)0, 4, current_offset + inner_offset);
							temp_bn.writeBlock((float)temp_aav.float_value, 4, current_offset + inner_offset + 4);
						}
						inner_offset += 8;
						break;
					default:
						break;
					}
				}
				insert_ii.num = b_counter - 1;
				insert_ii.offset = current_offset;
				if (delOrUnwrite == 1){
				}
				else{
					if ((current_offset + 2 * r_length) < 4096)
						temp_bn.writeBlock((int)2, 4, current_offset + r_length);
					else{
						//temp_bn.~bufferNode();
<<<<<<< HEAD
						bufferNode &temp_bn2 = getBlock(0, table_name, b_counter);
						temp_bn2.writeBlock((int)2, 4, 0);
=======
						bufferNode &temp_bn2 = getBlock(0,table_name,b_counter);
						temp_bn2.writeBlock((int)2,4,0);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
						int c = 2;
						c = 3;
					}
				}
				isEnd = true;	break;
			}
			current_offset += r_length;
		}
	}
	return insert_ii;
}

<<<<<<< HEAD
vector < vector < deleted_node > >& recordManager::delete_tuple(string table_name, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info> & join_ii) {

=======
vector < vector < deleted_node > >& recordManager::delete_tuple (string table_name, vector<TreeNode> &v_tn, vector<attrNode> &v_an, vector <index_info> & join_ii ) {
	
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
	vector<TreeNode>::iterator it_tn;
	vector<index_info>::iterator it_ii;
	vector<attrNode>::iterator it_an;

<<<<<<< HEAD
	int r_length = 4 + 4 * v_an.size() + v_an.back().offset + v_an.back().length;
=======
	int r_length = 4 + 4*v_an.size() + v_an.back().offset + v_an.back().length;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
	//r_length += 4 + 4*v_an.size();
	vector <int> v_indexNum;	vector <int>::iterator it_in;
	map<int, int> m;
	int indexNum = 0;
	int an_counter = 0;
<<<<<<< HEAD
	for (it_an = v_an.begin(); it_an != v_an.end(); it_an++, an_counter++) {
		if (it_an->hasIndex){
=======
	for(it_an = v_an.begin();it_an!=v_an.end();it_an++,an_counter++) {
		if(it_an->hasIndex){
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
			v_indexNum.push_back(an_counter);
			vector <deleted_node> v_dn;
			del_ind.push_back(v_dn);
			m[an_counter] = indexNum++;
		}
	}	//��ʼ��del_ind��m��¼�����������ԵĴ���
	count = 0;
<<<<<<< HEAD

	if (!join_ii.empty()) {
		bufferNode *p_temp_bn;
		//intersect(vector<index_info> &join_ii,vector < vector <index_info> > &join_ii);
		index_info temp_ii = { -1, -1 }; //�ж��Ƿ���ͬһ��block�ڣ�
		p_temp_bn = getBlockPointer(0, table_name, join_ii[0].num);

		for (it_ii = join_ii.begin(); it_ii != join_ii.end(); it_ii++) {
			if (it_ii->num != temp_ii.num){
				temp_ii = *it_ii;
				p_temp_bn = getBlockPointer(0, table_name, temp_ii.num);
			}
			temp_ii.offset = it_ii->offset;

			int delOrUnwrite = 0;
			p_temp_bn->readBlock(delOrUnwrite, 4, temp_ii.offset);
			if (delOrUnwrite)
				continue;
			if (!v_tn.empty()){
				bool notMatch = false;
				judge(notMatch, v_tn, v_an, p_temp_bn, temp_ii.offset);
				if (notMatch)
					continue;
			}
			delAndGetValue(del_ind, v_an, v_indexNum, m, p_temp_bn, temp_ii.offset);

		}
		cout << endl << ' ' << count << " record(s) affected" << endl;
		return del_ind;
=======

	if(!join_ii.empty()) {
		bufferNode *p_temp_bn;
		//intersect(vector<index_info> &join_ii,vector < vector <index_info> > &join_ii);
		index_info temp_ii = {-1,-1}; //判断是否在同一个block内；
		p_temp_bn = getBlockPointer(0, table_name, join_ii[0].num);

			for (it_ii = join_ii.begin(); it_ii != join_ii.end(); it_ii++) {
				if(it_ii->num != temp_ii.num){
					temp_ii = *it_ii;
					p_temp_bn = getBlockPointer(0,table_name,temp_ii.num);
				}
				temp_ii.offset = it_ii->offset;

				int delOrUnwrite = 0;
				p_temp_bn->readBlock(delOrUnwrite, 4, temp_ii.offset);
				if(delOrUnwrite)
					continue;
				if(!v_tn.empty()){
					bool notMatch = false;
					judge(notMatch,v_tn,v_an,p_temp_bn,temp_ii.offset);
					if(notMatch)
						continue;
				}
				delAndGetValue(del_ind,v_an,v_indexNum,m,p_temp_bn,temp_ii.offset);
				
			}
			cout << endl << ' ' << count << " record(s) affected" << endl;
			return del_ind;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
	}
	else {
		bool isEnd = false;
		int b_counter = 0;
<<<<<<< HEAD
		while (!isEnd) {
			bufferNode *p_temp_bn = getBlockPointer(0, table_name, b_counter++);
			int current_offset = 0;
			while (current_offset + r_length < 4096) {
				int delOrUnwrite = 0;
				p_temp_bn->readBlock(delOrUnwrite, 4, current_offset);
				if (delOrUnwrite == 1){
					current_offset += r_length;
					continue;
				}
				if (delOrUnwrite == 2){
					isEnd = true;
					break;
				}
				if (!v_tn.empty()){
					bool notMatch = false;
					judge(notMatch, v_tn, v_an, p_temp_bn, current_offset);
					if (notMatch){
						current_offset += r_length;
						continue;
					}
				}

				delAndGetValue(del_ind, v_an, v_indexNum, m, p_temp_bn, current_offset);
				current_offset += r_length;
=======
		while(!isEnd) {
			bufferNode *p_temp_bn = getBlockPointer(0,table_name,b_counter++);
			int current_offset = 0;
			while (current_offset + r_length < 4096) {
				int delOrUnwrite = 0;
				p_temp_bn->readBlock(delOrUnwrite,4,current_offset);
				if(delOrUnwrite==1){
					current_offset+=r_length;
					continue;
				}
				if(delOrUnwrite==2){
					isEnd = true;
					break;
				}
				if(!v_tn.empty()){
					bool notMatch = false;
					judge(notMatch,v_tn,v_an,p_temp_bn,current_offset);
					if(notMatch){
						current_offset+=r_length;
						continue;
					}
				}
				
				delAndGetValue(del_ind,v_an,v_indexNum,m,p_temp_bn,current_offset);
				current_offset+=r_length;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
			}
		}
		cout << endl << ' ' << count << " record(s) affected" << endl;
		return del_ind;
	}
}

vector <attrValue>& recordManager::select_attr(string table_name, vector <attrNode> &v_an, int num){
	bool isEnd = false;
	int b_counter = 0;
	int r_length = 4 + 4 * v_an.size() + v_an.back().offset + v_an.back().length;
	attrNode target_an = v_an[num];
	bufferNode *p_temp_bn = getBlockPointer(0, table_name, 0);
<<<<<<< HEAD
	while (!isEnd){
=======
	while(!isEnd){
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
		p_temp_bn = getBlockPointer(0, table_name, b_counter++);
		int current_offset = 0;

		while (current_offset + r_length < 4096){
			int delOrUnwrite = 0;
<<<<<<< HEAD
			p_temp_bn->readBlock(delOrUnwrite, 4, current_offset);
			if (delOrUnwrite == 1){
				current_offset += r_length;
=======
			p_temp_bn->readBlock(delOrUnwrite,4,current_offset);
			if(delOrUnwrite==1){
				current_offset+=r_length;
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
				continue;
			}
			if (delOrUnwrite == 2){
				isEnd = true;
				break;
			}
			attrValue t_av(target_an.type, target_an.attrName, b_counter - 1, current_offset);
			int isNull;
<<<<<<< HEAD
			p_temp_bn->readBlock(isNull, 4, current_offset + 4 + 4 * num + target_an.offset);
=======
			p_temp_bn->readBlock(isNull,4,current_offset+4+4*num+target_an.offset);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
			if (isNull) {
				t_av.value.isNull = true;
			}
			else{
				switch (target_an.type){
				case 1:
					int temp_int;
<<<<<<< HEAD
					p_temp_bn->readBlock(temp_int, 4, current_offset + 4 + 4 * (num + 1) + target_an.offset);
=======
					p_temp_bn->readBlock(temp_int,4,current_offset+4+4*(num+1)+target_an.offset);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
					t_av.value.n = temp_int;
					break;
				case 2:
				{
<<<<<<< HEAD
						  string temp_str;
						  p_temp_bn->readBlock(temp_str, target_an.length, current_offset + 4 + 4 * (num + 1) + target_an.offset);
						  t_av.value.s = temp_str;
						  break;
				}
				case 3:
					float temp_f;
					p_temp_bn->readBlock(temp_f, 4, current_offset + 4 + 4 * (num + 1) + target_an.offset);
=======
					string temp_str;
					p_temp_bn->readBlock(temp_str,target_an.length,current_offset+4+4*(num+1)+target_an.offset);
					t_av.value.s = temp_str;
					break;
				}
				case 3:
					float temp_f;
					p_temp_bn->readBlock(temp_f,4,current_offset+4+4*(num+1)+target_an.offset);
>>>>>>> 313ced2e1ed3e73b6e8146c54008286a3d1d93d9
					t_av.value.f = temp_f;
					break;
				default:
					break;
				}
			}
			sel_attr.push_back(t_av);
			current_offset += r_length;
		}
	}
	return sel_attr;
}

