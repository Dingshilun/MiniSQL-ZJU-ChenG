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
	if( !( Bfile = fopen( name, "rb+" ) ) && !( configFile = fopen( indexFileName+"Config", "rb+" ) ) )     //打开B+树文件  
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

ADDRESS BPlusTree :: newBplusNode()//在磁盘上分配一块B+树节点空间  
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
          
        //分裂根节点  
    }  
    insert( root ,tar );  
}

void BplusTree :: insert( ADDRESS current , Target& tar )  
{  
      BPlusNode x ;  
      readBplusNode( current, x );  
        
      int i;  
      for( i = 0; i < x.nkey && x.key[i] < tar.key; i++ );  
        
      if(i < x.nkey && x.isleaf && x.key[i] == tar.key )  //在B+树叶节点找到了相同关键字  
      {  
            //关键字插入重复  
            return ;  
      }  
        
      if( !x.isleaf )    //如果不是叶节点  
      {  
            BplusNode y;  
            readBplusNode( x.Pointer[i], y );  
              
            if( y.nkey == MAX_INT )     //如果x的子节点已满，则这个子节点分裂  
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
      else          //如果是叶节点,则直接将关键字插入key数组中  
      {  
              
            for( int j = x.nkey ; j > i ; j-- )  
            {  
                  x.key[j] = x.key[j-1] ;  
                  x.Pointer[j] = x.Pointer[j-1] ;  
            }  
            x.key[i] = tar.key ;  
            x.nkey++;  
              
            //将记录的地址赋给x.Pointer[i]  
              
            x.Pointer[i] = tar.index.offset + tar.index.num << 12;  
              
            writeBplusNode( current, x );  
              
      }  
        
}  

void BplusTree :: splitBplusNode( BplusNode &father, BplusNode &current , const int childnum )            //分裂满的B+树节点  
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
        
      if( current.isleaf )   //如果当前被分裂节点是叶子  
      {  
            current.nkey++;  
            t.Pointer[MAX_INT] = current.Pointer[MAX_INT];  
            current.Pointer[MAX_INT] = address ;  
      }  
        
      writeBplusNode( address, t );  
        
}

vector<index_info>& BplusTree :: searchBplusTree( Target& tar, ADDRESS Baddress ) const        //在B+树查询一个关键字  
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
    	 
    	if( i < a.nkey && a.isleaf && tar.key == a.key[i] )       //在B+树叶节点找到了等值的关键字  
    	{
    		Baddress = current;
    		if( tar.type == SINGLE )      
    	  	{	
    	  		result[iResult].num = a.Pointer[i]>>12;  
    	  		result[iResult].offset = (a.Pointer[i]<<20)>>20;              //返回该关键字所对应的记录的地址  
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
        
      if( i < x.nkey && x.key[i] == tar.key )  //在当前节点找到关键字  
      {  
              
            if( !x.isleaf )     //在内节点找到关键字  
            {  
                  BplusNode child;  
                  readBplusNode( x.Pointer[i], child );  
                    
                  if( child.isleaf )     //如果孩子是叶节点  
                  {  
                        if(child.nkey > MAX_INT/2 )      //情况A  
                        {  
                              x.key[i] = child.key[child.nkey - 2];  
                              child.nkey--;  
                                
                              writeBplusNode( current, x );  
                              writeBplusNode( x.Pointer[i], child );  
                                
                              return ;  
                        }  
                        else    //否则孩子节点的关键字数量不过半  
                        {  
                              if( i > 0 )      //有左兄弟节点  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode( x.Pointer[i-1], lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )        //情况B  
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
                                    else    //情况C  
                                    {  
                                          for( j = 0; j < child.nkey; j++ )  
                                          {  
                                                lbchild.key[lbchild.nkey + j ] = child.key[j];  
                                                lbchild.Pointer[lbchild.nkey + j ] = child.Pointer[j];  
                                          }  
                                          lbchild.nkey += child.nkey;  
                                            
                                          lbchild.Pointer[MAX_INT ] = child.Pointer[MAX_INT];  
                                            
                                          //释放child节点占用的空间x.Pointer[i]  
                                            
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
                              else      //只有右兄弟节点  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if(rbchild.nkey > MAX_INT/2 )        //情况D  
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
                                    else    //情况E  
                                    {  
                                          for( j = 0 ; j < rbchild.nkey ; j ++)  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.nkey += rbchild.nkey ;  
                                            
                                          child.Pointer[MAX_INT] = rbchild.Pointer[MAX_INT];  
                                            
                                          //释放rbchild占用的空间x.Pointer[i+1]  
                                            
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
                  else      //情况F  
                  {  
                          
                        //找到key在B+树叶节点的左兄弟关键字,将这个关键字取代key的位置  
                          
                        Target ttar;  
                        ttar.key = tar.key;  
                        ADDRESS Baddress;
                        result = searchBplusTree( ttar, Baddress );  
                          
                        BplusNode last;  
                          
                        readBplusNode( Baddress, last );  
                          
                        x.key[i] = last.key[last.nkey - 2 ];  
                          
                        writeBplusNode(current ,x);  
                          
                      
                        if(child.nkey > MAX_INT/2 )        //情况H  
                        {  
                                
                        }  
                        else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
                        {  
                              if(i > 0 )  //x.key[i]有左兄弟  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //情况I  
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
                                    else        //情况J  
                                    {  
                                          lbchild.key[lbchild.nkey] = x.key[i-1];   //将孩子节点复制到其左兄弟的末尾  
                                          lbchild.nkey ++;  
                                            
                                          for(j = 0 ; j < child.nkey ; j++)      //将child节点拷贝到lbchild节点的末尾,  
                                          {  
                                                lbchild.key[lbchild.nkey + j] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          }  
                                          lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          lbchild.nkey += child.nkey ;        //已经将child拷贝到lbchild节点  
                                            
                                            
                                          //释放child节点的存储空间,x.Pointer[i]  
                                            
                                            
                                          //将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
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
                              else        //否则x.key[i]只有右兄弟  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )     //情况K  
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
                                    else        //情况L  
                                    {  
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          for(j = 0; j < rbchild.nkey ; j++)     //将rbchild节点合并到child节点后  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                            
                                          child.nkey += rbchild.nkey;  
                                            
                                          //释放rbchild节点所占用的空间,x,Pointer[i+1]  
                                            
                                          for(j = i ;j < x.nkey - 1 ; j++ )    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
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
            else  //情况G  
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
      else        //在当前节点没找到关键字     
      {  
            if(!x.isleaf )    //没找到关键字,则关键字必然包含在以Pointer[i]为根的子树中  
            {  
                  BplusNode child;  
                  readBplusNode(x.Pointer[i] ,child );  
                    
                  if(!child.isleaf )      //如果其孩子节点是内节点  
                  {  
                        if(child.nkey > MAX_INT/2 )        //情况H  
                        {  
                                
                        }  
                        else          //否则孩子节点的关键字数量不过半,则将兄弟节点的某一个关键字移至孩子  
                        {  
                              if(i > 0 )  //x.key[i]有左兄弟  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //情况I  
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
                                    else        //情况J  
                                    {  
                                          lbchild.key[lbchild.nkey] = x.key[i-1];   //将孩子节点复制到其左兄弟的末尾  
                                          lbchild.nkey ++;  
                                            
                                          for(j = 0 ; j < child.nkey ; j++)      //将child节点拷贝到lbchild节点的末尾,  
                                          {  
                                                lbchild.key[lbchild.nkey + j] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          }  
                                          lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j];  
                                          lbchild.nkey += child.nkey ;        //已经将child拷贝到lbchild节点  
                                            
                                            
                                          //释放child节点的存储空间,x.Pointer[i]  
                                            
                                            
                                          //将找到关键字的孩子child与关键字左兄弟的孩子lbchild合并后,将该关键字前移,使当前节点的关键字减少一个  
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
                              else        //否则x.key[i]只有右兄弟  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )     //情况K  
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
                                    else        //情况L  
                                    {  
                                          child.key[child.nkey] = x.key[i];  
                                          child.nkey ++;  
                                            
                                          for(j = 0; j < rbchild.nkey ; j++)     //将rbchild节点合并到child节点后  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                          }  
                                          child.Pointer[child.nkey +j] = rbchild.Pointer[j];  
                                            
                                          child.nkey += rbchild.nkey;  
                                            
                                          //释放rbchild节点所占用的空间,x,Pointer[i+1]  
                                            
                                          for(j = i ;j < x.nkey - 1 ; j++ )    //当前将关键字之后的关键字左移一位,使该节点的关键字数量减一  
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
                  else  //否则其孩子节点是外节点  
                  {  
                        if(child.nkey > MAX_INT/2 )  //情况M  
                        {  
                                
                        }  
                        else        //否则孩子节点不到半满  
                        {  
                              if( i > 0 ) //有左兄弟  
                              {  
                                    BplusNode lbchild;  
                                    readBplusNode(x.Pointer[i-1] ,lbchild );  
                                      
                                    if( lbchild.nkey > MAX_INT/2 )       //情况N  
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
                                    else        //情况O  
                                    {  
                                            
                                          for( j = 0 ; j < child.nkey ; j++ )        //与左兄弟孩子节点合并  
                                          {  
                                                lbchild.key[lbchild.nkey + j ] = child.key[j] ;  
                                                lbchild.Pointer[lbchild.nkey + j] = child.Pointer[j] ;   
                                          }  
                                          lbchild.nkey += child.nkey ;  
                                            
                                          lbchild.Pointer[MAX_INT] = child.Pointer[MAX_INT];  
                                            
                                          //释放child占用的空间x.Pointer[i]  
                                            
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
                              else        //否则只有右兄弟  
                              {  
                                    BplusNode rbchild;  
                                    readBplusNode(x.Pointer[i+1] ,rbchild );  
                                      
                                    if( rbchild.nkey > MAX_INT/2 )       //情况P  
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
                                    else        //情况Q  
                                    {  
                                          for(j = 0 ; j < rbchild.nkey ; j ++)  
                                          {  
                                                child.key[child.nkey + j] = rbchild.key[j];  
                                                child.Pointer[child.nkey + j] = rbchild.Pointer[j];  
                                          }  
                                          child.nkey += rbchild.nkey;  
                                          child.Pointer[MAX_INT] = rbchild.Pointer[MAX_INT];  
                                            
                                          //释放rbchild占用的空间x.Pointer[i+1]  
                                            
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

void BPlusTree :: deleteBplusTree( Target& tar )    //在B+中删除一个关键字  
{  
      Delete( root, tar );  
        
      BplusNode rootnode;  
      readBplusNode( root, rootnode );  
        
      if( !rootnode.isleaf && rootnode.nkey == 0 )    //如果删除关键字后根节点不是叶节点，并且关键字数量为0时根节点也应该被删除  
      {  
            //释放ROOT节点占用的空间  
            root = rootnode.Pointer[0];         //根节点下移,B+树高度减1               
      }  
        
} 

void  BPlusTree :: enumLeafKey()    //依次枚举B+树叶节点的所有关键字  
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
