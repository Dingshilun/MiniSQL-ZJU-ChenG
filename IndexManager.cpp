//The format of the name of the index file is: I_xxx(table name)_xxx(attribute name).index
#include <IndexManager.h>

bool IndexManager :: createIndex( Target tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;
	if( indexTable.count(indexFileName) == 1 )
	{
		cout << "the index has already exist!" << endl;
		return false;
	}
	else
	{
		if( access( indexFileName, F_OK ) )
		{
			cout << "the index has already exist!" << endl;
			return false;
		}
		BplusTree Btree = new BplusTree();
		BufferNode bf_node = getBlock( INDEX, indexFileName, 0 );
		Btree.buildBplusTree( indexFileName, sizeof(int), bf_node );
		indexTable[indexFileName] = Btree;
		return true;
	}
}

vector<index_info>& IndexManager :: search( Target tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;

	vector<index_info> result;
	result[0].num = -1;
	result[0].offset = -1;

	if( indexTable.count(indexFileName) == 1 )
	{
		Btree = map[indexFileName];
		result = Btree.searchBplusTree( tar );
	}
	else if( access( indexFileName, F_OK ) )
	{
		Btree = new Btree( indexFileName );
		map[indexFileName] = Btree;
		ADDRESS tmp;
		result = Btree.searchBplusTree( tar, tmp );
	}
	else
		cout << "The index file does not exist!" << endl;
	return result;
} 

void IndexManager :: insert( Target& tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;
	if( indexTable.count(indexFileName) == 1 )
	{
		Btree = map[indexFileName];
		Btree.insertBplusTree( tar );
	}
	else if( access( indexFileName, F_OK ) )
	{
		Btree = new Btree( indexFileName );
		map[indexFileName] = Btree;
		ADDRESS tmp;
		Btree.insertBplusTree( tar, tmp );
	}
	else
		cout << "The index file does not exist!" << endl;
}

void IndexManager :: deleteKey( Target& tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;
	if( indexTable.count(indexFileName) == 1 )
	{
		Btree = map[indexFileName];
		Btree.deleteBplusTree( tar );
	}
	else if( access( indexFileName, F_OK ) )
	{
		Btree = new Btree( indexFileName );
		map[indexFileName] = Btree;
		ADDRESS tmp;
		Btree.deleteBplusTree( tar, tmp );
	}
	else
		cout << "The index file does not exist!" << endl;
}

bool IndexManager :: deleteIndex( Target& tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;
	if( indexTable.count(indexFileName) == 1 )
		indexTable.erase(indexFileName);
		
	if( access( indexFileName, F_OK ) )
	{
		if( remove( indexFileName ) != 0 && remove( indexFileName+"Config" ) != 0 )
		{
			cout << "Fail to delete the index file!" << endl;
			return false;
		}
		else
			return true;
	}
	else
	{
		cout << "The index you want to delete dose not exist!" << endl;
		return false;
	}
}

bool IndexManager :: exisstIndex( Target& tar )
{
	string indexFileName = "I_";
	indexFileName = indexFileName + tar.table + tar.indexName;
		
	if( access( indexFileName, F_OK ) )
		return true;
	else
		return false;
}

BplusTree :: BplusTree( string name, BufferNode& bf_node )
{
	if( !( Bfile = fopen( name, "rb+" ) ) && !( configFile = fopen( indexFileName+"Config", "rb+" ) ) )     //��B+���ļ�  
        cout << "Cannot open the index file!" << endl;
    else
    {
    	temp_bf = bf_node;
    	indexFileName = name;
		//BplusNode temp;
		//readBplusNode( 0, temp );
		temp_bf = getBlock( INDEX, name+"Config", 0 ); //the block number of the root node is stored in another config file
		temp_bf.readBlock( root, sizeof(ADDRESS), 0 ); 
    }
}

BplusTree :: ~BplusTree()
{
	temp_bf = getBlock( INDEX, indexFileName+"Config", 0 );
	temp_bf.writeBlock( root, sizeof(ADDRESS), 0 );
	fclose(Bfile);
	fclose(configFile);
	//write root back to the config file
}

ADDRESS BPlusTree :: newBplusNode()//�ڴ����Ϸ���һ��B+���ڵ�ռ�  
{  
    fseek( Bfile, 0, SEEK_END );  
    char* Block = new char[4096];
    fwrite( Block, BLOCK_SIZE, 1 ,Bfile);
    fseek( Bfile, 0, SEEK_END );       
    return  ( ftell( Bfile )/BLOCK_SIZE - 1 );  
}  

void BplusTree :: readBplusNode( ADDRESS add, BplusNode& node )
{
	temp_bf = getBlock( INDEX, indexFileName, add );
	temp_bf.readBlock( node, 0, sizeof(BplusNode) );
}

void BplusTree :: writeBplusNode( ADDRESS add, BplusNode& node )
{
	temp_bf = getBlock( INDEX, indexFileName, add );
	temp_bf.writeBlock( node, 0, sizeof(BplusNode) );
}

bool BplusTree :: buildBplusTree( String name, int keyType, BufferNode& bf_node )   //build a empty B+ tree 
{ 
	if( access(name, F_OK ) )
	{
		cout << "The index file has already existed!" << endl;
		return false;
	}
	else
	{
		indexFileName = name;
		//keyNumber = MAX_INT; 
		if( ( Bfile = fopen( indexFileName, "wb+") ) ) 
		{
			if( ( configFile = fopen( indexFileName+"Config", "wb+") ) ) 
			{
				temp_bf = bf_node;

				root = newBplusNode();
    			BplusNode r;  
    			r.Pointer[MAX_INT] = 0;
    			r.nkey = 0;  
    			r.isleaf = true;
				
    			writeBplusNode( root, r );
    			
    			fseek( configFile, 0, SEEK_END );  
    			char* Block = new char[4096];
    			fwrite( Block, BLOCK_SIZE, 1 ,Bfile);
    			temp_bf = getBlock( INDEX, indexFileName+"Config", 0 );
				temp_bf.writeBlock( root, sizeof(ADDRESS), 0 );

				return true;
    		}
    		else
    		{
    			cout << "Cannot create the index config file!" << endl;
    			return false
    		}
    	}  
    	else
    	{
    		cout << "Cannot create the index file!" << endl;
    		return false;
    	}
    }
}  

void BplusTree :: insertBplusTree( Target& tar )
{
	BPlusNode r;  
    readBplusNode( root, r );  
      
    if( r.nkey == INT_MAX )  
    {  
        BplusNode newroot;  
        newroot.nkey = 0;  
        newroot.isleaf = false;  
        newroot.Pointer[0] = root;  
          
        splitBplusNode( newroot, r, 0 );  
        writeBplusNode( root ,r );  
          
        root = newBplusNode();  
          
        writeBplusNode( root ,newroot );  
          
        //���Ѹ��ڵ�  
    }  
    insert( root ,tar );  
}

void BplusTree :: insert( ADDRESS current , Target& tar )  
{  
      BPlusNode x ;  
      readBplusNode( current, x );  
        
      int i;  
      for( i = 0; i < x.nkey && x.key[i] < tar.key; i++ );  
        
      if(i < x.nkey && x.isleaf && x.key[i] == tar.key )  //��B+��Ҷ�ڵ��ҵ�����ͬ�ؼ���  
      {  
            //�ؼ��ֲ����ظ�  
            return ;  
      }  
        
      if( !x.isleaf )    //�������Ҷ�ڵ�  
      {  
            BplusNode y;  
            readBplusNode( x.Pointer[i], y );  
              
            if( y.nkey == MAX_INT )     //���x���ӽڵ�������������ӽڵ����  
            {  
                  splitBplusNode( x, y, i );  
                  writeBplusNode( current, x );  
                  writeBplusNode( x.Pointer[i], y );  
            }  
            if( tar.key <= x.key[i] || i == x.nkey )  
            {  
                  insert( x.Pointer[i], tar );  
            }  
            else  
            {  
                  insert (x.Pointer[i+1], tar );  
            }  
              
      }  
      else          //�����Ҷ�ڵ�,��ֱ�ӽ��ؼ��ֲ���key������  
      {  
              
            for( int j = x.nkey ; j > i ; j-- )  
            {  
                  x.key[j] = x.key[j-1] ;  
                  x.Pointer[j] = x.Pointer[j-1] ;  
            }  
            x.key[i] = tar.key ;  
            x.nkey++;  
              
            //����¼�ĵ�ַ����x.Pointer[i]  
              
            x.Pointer[i] = tar.index.offset + tar.index.num << 12;  
              
            writeBplusNode( current, x );  
              
      }  
        
}  

void BplusTree :: splitBplusNode( BplusNode &father, BplusNode &current , const int childnum )            //��������B+���ڵ�  
{  
      int half = MAX_INT/2 ;  
        
      int i ;  
        
      for(i = father.nkey ; i > childnum ; i -- )  
      {  
            father.key[i] = father.key[i-1] ;  
            father.Pointer[i+1] = father.Pointer[i];  
      }  
      father.nkey ++;  
        
      BPlusNode t;  
        
      ADDRESS address = newBplusNode();  
        
      father.key[childnum] = current.key[half] ;  
      father.Pointer[childnum + 1] = address;  
        
      for( i = half + 1; i < MAX_INT; i++ )  
      {  
            t.key[i-half-1] = current.key[i] ;  
            t.Pointer[i-half-1] = current.Pointer[i];  
      }  
        
      t.nkey = MAX_INT - half - 1;  
      t.Pointer[t.nkey] = current.Pointer[MAX_INT];  
        
      t.isleaf = current.isleaf;  
        
      current.nkey = half ;  
        
      if( current.isleaf )   //�����ǰ�����ѽڵ���Ҷ��  
      {  
            current.nkey++;  
            t.Pointer[MAX_INT] = current.Pointer[MAX_INT];  
            current.Pointer[MAX_INT] = address ;  
      }  
        
      writeBplusNode( address, t );  
        
}

vector<index_info>& BplusTree :: searchBplusTree( Target& tar, ADDRESS Baddress ) const        //��B+����ѯһ���ؼ���  
{  
    int i, iResult = 0;
    int flagNode = -1, flagIndex = -1;  
    
    vector<index_info> result;
    result[0].num = -1;
    reuslt[0].offset = -1
    BplusNode a;  
    ADDRESS current = root;  
    ADDRESS last;
     
    do  
    {  
    	readBplusNode( current, a );  
    	  
    	for( i = 0; i < a.nkey && tar.key > a.key[i]; i++ );  
    	 
    	if( i < a.nkey && a.isleaf && tar.key == a.key[i] )       //��B+��Ҷ�ڵ��ҵ��˵�ֵ�Ĺؼ���  
    	{
    		Baddress = current;
    		if( tar.type == SINGLE )      
    	  	{	
    	  		result[iResult].num = a.Pointer[i]>>12;  
    	  		result[iResult].offset = (a.Pointer[i]<<20)>>20;              //���ظùؼ�������Ӧ�ļ�¼�ĵ�ַ  
    	  	}
    	  	else if( tar.type == NOTLESS || NOTLARGER )
    	  	{
    	  		flagIndex = i;
    	  		flagNode = current;
    	  	}
    	}       
    	last = current;
    	if( i == MAX_INT )	
    		current = a.Pointer[i]; 
    }while(!a.isleaf)
    if( tar.type == NOTLESS || tar.type == LARGER )
    {
    	if( flagIndex < 0 )
    	{
    		flagIndex = i;
    		flagNode = current;
    	}
    	for( ; i < a.nkey; i++ )
    	{
    		result[iResult].num = a.Pointer[i]>>12;  
    		result[iResult].offset = (a.Pointer[i]<<20)>>20;
    		iResult++;
    	}
    	while( i == MAX_INT )
    	{
    		current = a.Pointer[i];
    		readBplusNode( current, a );
    		for( i = 0; i < a.nkey; i++ )
    		{
    			result[iResult].num = a.Pointer[i]>>12;  
    			result[iResult].offset = (a.Pointer[i]<<20)>>20;
    			iResult++;	
    		}
    	}   	
    }
    else if( tar.type == NOTLARGER || tar.type == LESS )
    {
    	if( flagIndex < 0 )
    	{
    		if( last != current )
    		{
    			flagIndex = MAX_INT;
    			flagNode = last;
    		}
    		else
    		{
    			flagNode = current;
    			flagIndex = i - 1;
    		}
    	}
    	BplusNode head;  
      
   	readBplusNode( root, head );  
   	
    	while(!head.isleaf )  
    	{  
        		readBplusNode( head.Pointer[0], head );  
    	}
    	current = head;
    	i = 0;
    	while( (current != endNode) || ( current == endNode && i <= endIndex ) )
    	{
    		if( i == MAX_INT )
    		{
    			current = a.Pointer[i];
    			readBplusNode( current, a );
    		}
    		else
    		{
    			result[iResult].num = a.Pointer[i]>>12;  
    	 		result[iResult].offset = (a.Pointer[i]<<20)>>20;
    	 		iResult++;	
    	 		i++;
    		}
    	}
    
    return result;
}    

void BPlusTree :: Delete( ADDRESS current ,Target& tar )  
{  
      int i, j;  
        
      BplusNode x;  
      readBplusNode( current, x );  
  
        
      for( i = 0; i < x.nkey && tar.key > x.key[i]; i++ );  
        
      if( i < x.nkey && x.key[i] == tar.key )  //�ڵ�ǰ�ڵ��ҵ��ؼ���  
      {  
              
            if( !x.isleaf )     //���ڽڵ��ҵ��ؼ���  
            {  
                  BplusNode child;  
                  readBplusNode( x.Pointer[i], child );  
                    
                  if( child.isleaf )     //���������Ҷ�ڵ�  
                  {  
                        if(child.nkey > MAX_INT/2 )      //���A  
                        {  
                              x.key[i] = child.key[child.nkey - 2];  
                              child.nkey--;  
                                
                              writeBplusNode( current, x );  
                              writeBplusNode( x.Pointer[i], child );  
                                
                              return ;  
                        }  
                        else    //�����ӽڵ�Ĺؼ�������������  
                        {  
                              if( i > 0 )      //�����ֵܽڵ�  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode( x.Pointer[i-1], lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )        //���B  
                                    {  
                                          for( j = child.nkey; j > 0; j-- )  
                                          {  
                                                child.key[j] = child.key[j-1];  
                                                child.Pointer[j] = child.Pointer[j-1];  
                                          }  
                                            
                                          child.key[0] = x.key[i-1];  
                                          child.Pointer[0] = lbchild.Pointer[lbchild.nkey-1];  
                                            
                                          child.nkey++;  
                                            
                                          lbchild.nkey--;  
                                            
                                          x.key[i-1] = lbchild.key[lbchild.nkey-1];  
                                          x.key[i] = child.key[child.nkey-2];  
                                            
                                          writeBplusNode( current, x );  
                                          writeBplusNode( x.Pointer[i-1], lbchild );  
                                          writeBplusNode( x.Pointer[i], child );  
                                            
                                    }  
                                    else    //���C  
                                    {  
                                          for( j = 0; j < child.nkey; j++ )  
                                          {  
                                                lbchild.key[lbchild.nkey + j ] = child.key[j];  
                                                lbchild.Pointer[lbchild.nkey + j ] = child.Pointer[j];  
                                          }  
                                          lbchild.nkey += child.nkey;  
                                            
                                          lbchild.Pointer[MAX_INT ] = child.Pointer[MAX_INT];  
                                            
                                          //�ͷ�child�ڵ�ռ�õĿռ�x.Pointer[i]  
                                            
                                          for( j = i - 1; j < x.nkey - 1; j++ )  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey--;  
  
                                          x.key[i-1] = lbchild.key[lbchild.nkey-2];  
                                            
                                          writeBplusNode( current, x );  
                                          writeBplusNode( x.Pointer[i-1], lbchild );  
                                            
                                          i--;  
                                            
                                    }  
                                      
                                      
                              }  
                              else      //ֻ�����ֵܽڵ�  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if(rbchild.nkey > MAX_INT/2 )        //���D  
                                    {  
                                          x.key[i] = rbchild.key[0];  
                                          child.key[child.nkey] = rbchild.key[0];  
                                          child.Pointer[child.nkey] = rbchild.Pointer[0];  
                                          child.nkey ++;  
                                            
                                          for( j = 0 ; j < rbchild.nkey - 1 ; j ++)  
                                          {  
                                                rbchild.key[j] = rbchild.key[j+1];  
                                                rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          }  
                                            
                                          rbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                          writeBplusNode(x.Pointer[i+1] ,rbchild );  
                                            
                                    }  
                                    else    //���E  
                                    {  
                                          for( j = 0 ; j < rbchild.nkey ; j ++)  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.nkey += rbchild.nkey ;  
                                            
                                          child.Pointer[MAX_INT] = rbchild.Pointer[MAX_INT];  
                                            
                                          //�ͷ�rbchildռ�õĿռ�x.Pointer[i+1]  
                                            
                                          for( j = i  ; j < x.nkey - 1; j ++)  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                            
                                    }  
                                      
                              }  
                                
                      }  
                        
                  }  
                  else      //���F  
                  {  
                          
                        //�ҵ�key��B+��Ҷ�ڵ�����ֵܹؼ���,������ؼ���ȡ��key��λ��  
                          
                        Target ttar;  
                        ttar.key = tar.key;  
                        ADDRESS Baddress;
                        result = searchBplusTree( ttar, Baddress );  
                          
                        BplusNode last;  
                          
                        readBplusNode( Baddress, last );  
                          
                        x.key[i] = last.key[last.nkey - 2 ];  
                          
                        writeBplusNode(current ,x);  
                          
                      
                        if(child.nkey > MAX_INT/2 )        //���H  
                        {  
                                
                        }  
                        else          //�����ӽڵ�Ĺؼ�������������,���ֵܽڵ��ĳһ���ؼ�����������  
                        {  
                              if(i > 0 )  //x.key[i]�����ֵ�  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //���I  
                                    {  
                                          for( j = child.nkey ; j > 0 ; j -- )  
                                          {  
                                                child.key[j] = child.key[j-1];  
                                                child.Pointer[j+1] = child.Pointer[j];  
                                          }  
                                          child.Pointer[1] = child.Pointer[0];  
                                          child.key[0] = x.key[i-1] ;  
                                          child.Pointer[0] = lbchild.Pointer[lbchild.nkey];  
                                            
                                          child.nkey ++;  
                                            
                                          x.key[i-1] = lbchild.key[lbchild.nkey-1] ;  
                                          lbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                    }  
                                    else        //���J  
                                    {  
                                          lbchild.key[lbchild.nkey] = x.key[i-1];   //�����ӽڵ㸴�Ƶ������ֵܵ�ĩβ  
                                          lbchild.nkey ++;  
                                            
                                          for(j = 0 ; j < child.nkey ; j++)      //��child�ڵ㿽����lbchild�ڵ��ĩβ,  
                                          {  
                                                lbchild.key[lbchild.nkey + j] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          }  
                                          lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          lbchild.nkey += child.nkey ;        //�Ѿ���child������lbchild�ڵ�  
                                            
                                            
                                          //�ͷ�child�ڵ�Ĵ洢�ռ�,x.Pointer[i]  
                                            
                                            
                                          //���ҵ��ؼ��ֵĺ���child��ؼ������ֵܵĺ���lbchild�ϲ���,���ùؼ���ǰ��,ʹ��ǰ�ڵ�Ĺؼ��ּ���һ��  
                                          for(j = i - 1  ; j < x.nkey - 1 ; j++)  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                            
                                          i --;  
                                            
                                    }  
                                      
                              }  
                              else        //����x.key[i]ֻ�����ֵ�  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )     //���K  
                                    {  
                                            
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          child.Pointer[child.nkey] = rbchild.Pointer[0];  
                                          x.key[i] = rbchild.key[0];  
                                            
                                          for( j = 0 ; j < rbchild.nkey -1 ; j++)  
                                          {  
                                                rbchild.key[j] = rbchild.key[j+1];  
                                                rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          }  
                                          rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          rbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                          writeBplusNode(x.Pointer[i+1] ,rbchild );  
                                            
                                    }  
                                    else        //���L  
                                    {  
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          for(j = 0; j < rbchild.nkey ; j++)     //��rbchild�ڵ�ϲ���child�ڵ��  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                            
                                          child.nkey += rbchild.nkey;  
                                            
                                          //�ͷ�rbchild�ڵ���ռ�õĿռ�,x,Pointer[i+1]  
                                            
                                          for(j = i ;j < x.nkey - 1 ; j++ )    //��ǰ���ؼ���֮��Ĺؼ�������һλ,ʹ�ýڵ�Ĺؼ���������һ  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                            
                                    }  
                                      
                              }  
                        }  
                          
                  }  
                    
                  Delete(x.Pointer[i] ,tar );  
                    
            }  
            else  //���G  
            {  
                  for( j = i ; j < x.nkey - 1 ; j ++ )  
                  {  
                        x.key[j] = x.key[j+1];  
                        x.Pointer[j] = x.Pointer[j+1];  
                  }  
                  x.nkey-- ;  
                    
                  writeBplusNode(current ,x);  
                    
                  return ;  
            }  
              
      }  
      else        //�ڵ�ǰ�ڵ�û�ҵ��ؼ���     
      {  
            if(!x.isleaf )    //û�ҵ��ؼ���,��ؼ��ֱ�Ȼ��������Pointer[i]Ϊ����������  
            {  
                  BplusNode child;  
                  readBplusNode(x.Pointer[i] ,child );  
                    
                  if(!child.isleaf )      //����亢�ӽڵ����ڽڵ�  
                  {  
                        if(child.nkey > MAX_INT/2 )        //���H  
                        {  
                                
                        }  
                        else          //�����ӽڵ�Ĺؼ�������������,���ֵܽڵ��ĳһ���ؼ�����������  
                        {  
                              if(i > 0 )  //x.key[i]�����ֵ�  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //���I  
                                    {  
                                          for( j = child.nkey ; j > 0 ; j -- )  
                                          {  
                                                child.key[j] = child.key[j-1];  
                                                child.Pointer[j+1] = child.Pointer[j];  
                                          }  
                                          child.Pointer[1] = child.Pointer[0];  
                                          child.key[0] = x.key[i-1] ;  
                                          child.Pointer[0] = lbchild.Pointer[lbchild.nkey];  
                                            
                                          child.nkey ++;  
                                            
                                          x.key[i-1] = lbchild.key[lbchild.nkey-1] ;  
                                          lbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                    }  
                                    else        //���J  
                                    {  
                                          lbchild.key[lbchild.nkey] = x.key[i-1];   //�����ӽڵ㸴�Ƶ������ֵܵ�ĩβ  
                                          lbchild.nkey ++;  
                                            
                                          for(j = 0 ; j < child.nkey ; j++)      //��child�ڵ㿽����lbchild�ڵ��ĩβ,  
                                          {  
                                                lbchild.key[lbchild.nkey + j] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          }  
                                          lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          lbchild.nkey += child.nkey ;        //�Ѿ���child������lbchild�ڵ�  
                                            
                                            
                                          //�ͷ�child�ڵ�Ĵ洢�ռ�,x.Pointer[i]  
                                            
                                            
                                          //���ҵ��ؼ��ֵĺ���child��ؼ������ֵܵĺ���lbchild�ϲ���,���ùؼ���ǰ��,ʹ��ǰ�ڵ�Ĺؼ��ּ���һ��  
                                          for(j = i - 1  ; j < x.nkey - 1 ; j++)  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                            
                                          i --;  
                                            
                                    }  
                                      
                              }  
                              else        //����x.key[i]ֻ�����ֵ�  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )     //���K  
                                    {  
                                            
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          child.Pointer[child.nkey] = rbchild.Pointer[0];  
                                          x.key[i] = rbchild.key[0];  
                                            
                                          for( j = 0 ; j < rbchild.nkey -1 ; j++)  
                                          {  
                                                rbchild.key[j] = rbchild.key[j+1];  
                                                rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          }  
                                          rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          rbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                          writeBplusNode(x.Pointer[i+1] ,rbchild );  
                                            
                                    }  
                                    else        //���L  
                                    {  
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          for(j = 0; j < rbchild.nkey ; j++)     //��rbchild�ڵ�ϲ���child�ڵ��  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                            
                                          child.nkey += rbchild.nkey;  
                                            
                                          //�ͷ�rbchild�ڵ���ռ�õĿռ�,x,Pointer[i+1]  
                                            
                                          for(j = i ;j < x.nkey - 1 ; j++ )    //��ǰ���ؼ���֮��Ĺؼ�������һλ,ʹ�ýڵ�Ĺؼ���������һ  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x);  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                            
                                    }  
                                      
                              }  
                        }  
                  }  
                  else  //�����亢�ӽڵ�����ڵ�  
                  {  
                        if(child.nkey > MAX_INT/2 )  //���M  
                        {  
                                
                        }  
                        else        //�����ӽڵ㲻������  
                        {  
                              if( i > 0 ) //�����ֵ�  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //���N  
                                    {  
                                          for(j = child.nkey ; j > 0  ; j--)  
                                          {  
                                                child.key[j] = child.key[j-1];  
                                                child.Pointer[j] = child.Pointer[j-1];  
                                          }  
                                          child.key[0] = x.key[i-1];  
                                          child.Pointer[0] = lbchild.Pointer[lbchild.nkey-1];  
                                          child.nkey ++;  
                                          lbchild.nkey --;  
                                            
                                          x.key[i-1] = lbchild.key[lbchild.nkey-1];  
                                            
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                          writeBplusNode(current ,x );  
                                            
                                    }  
                                    else        //���O  
                                    {  
                                            
                                          for( j = 0 ; j < child.nkey ; j++ )        //�����ֵܺ��ӽڵ�ϲ�  
                                          {  
                                                lbchild.key[lbchild.nkey + j ] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j] ;   
                                          }  
                                          lbchild.nkey += child.nkey ;  
                                            
                                          lbchild.Pointer[MAX_INT] = child.Pointer[MAX_INT];  
                                            
                                          //�ͷ�childռ�õĿռ�x.Pointer[i]  
                                            
                                          for( j = i - 1; j < x.nkey - 1 ; j ++ )  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                            
                                          x.nkey --;  
                                            
                                          writeBplusNode(x.Pointer[i-1] ,lbchild );  
                                          writeBplusNode(current ,x );  
                                            
                                          i --;  
                                            
                                    }  
                                      
                              }  
                              else        //����ֻ�����ֵ�  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )       //���P  
                                    {  
                                          x.key[i] = rbchild.key[0] ;  
                                          child.key[child.nkey] = rbchild.key[0];  
                                          child.Pointer[child.nkey] = rbchild.Pointer[0];  
                                          child.nkey ++;  
                                            
                                          for(j = 0 ; j < rbchild.nkey - 1 ; j ++)  
                                          {  
                                                rbchild.key[j] = rbchild.key[j+1];  
                                                rbchild.Pointer[j] = rbchild.Pointer[j+1];  
                                          }  
                                          rbchild.nkey --;  
                                            
                                          writeBplusNode(current ,x );  
                                          writeBplusNode(x.Pointer[i+1] ,rbchild );  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                            
                                    }  
                                    else        //���Q  
                                    {  
                                          for(j = 0 ; j < rbchild.nkey ; j ++)  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey + j] = rbchild.Pointer[j];  
                                          }  
                                          child.nkey += rbchild.nkey;  
                                          child.Pointer[MAX_INT] = rbchild.Pointer[MAX_INT];  
                                            
                                          //�ͷ�rbchildռ�õĿռ�x.Pointer[i+1]  
                                            
                                          for(j = i ; j < x.nkey - 1 ; j ++ )  
                                          {  
                                                x.key[j] = x.key[j+1];  
                                                x.Pointer[j+1] = x.Pointer[j+2];  
                                          }  
                                          x.nkey --;  
                                            
                                          writeBplusNode(current ,x );  
                                          writeBplusNode(x.Pointer[i] ,child );  
                                            
                                            
                                    }  
                                      
                              }  
                                
                        }  
                          
                  }  
                    
                  Delete(x.Pointer[i] ,tar );  
            }  
              
              
      }  
        
        
}

void BPlusTree :: deleteBplusTree( Target& tar )    //��B+��ɾ��һ���ؼ���  
{  
      Delete( root, tar );  
        
      BplusNode rootnode;  
      readBplusNode( root, rootnode );  
        
      if( !rootnode.isleaf && rootnode.nkey == 0 )    //���ɾ���ؼ��ֺ���ڵ㲻��Ҷ�ڵ㣬���ҹؼ�������Ϊ0ʱ���ڵ�ҲӦ�ñ�ɾ��  
      {  
            //�ͷ�ROOT�ڵ�ռ�õĿռ�  
            root = rootnode.Pointer[0];         //���ڵ�����,B+���߶ȼ�1               
      }  
        
} 

void  BPlusTree :: enumLeafKey()    //����ö��B+��Ҷ�ڵ�����йؼ���  
{  
      BplusNode head;  
        
      readBplusNode( root, head );  
        
      while(!head.isleaf )  
      {  
            readBplusNode( head.Pointer[0], head );  
      }  
        
      while(1)  
      {  
            for(int i = 0 ; i < head.nkey ; i ++)  
                  printf("%d\n",head.key[i] );  
              
            if(head.Pointer[MAX_INT] == 0 )  
                  break;  
              
            readBplusNode(head.Pointer[MAX_INT] ,head );  
      }  
        
}  

