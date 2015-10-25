/*
pin为true的block在buffer满时不会被替换
getBlock中的offset指第几个block
其余offset(block内的)全部指字节
length也是字节
type:table=0,index=1
*/

#ifndef __BUFFERMANAGER__H
#define __BUFFERMANAGER__H

#define  BLOCK_SIZE 4096
#define  MAX_BLOCK 1024

#include <string>
#include <vector>

enum Type{
	TABLE, INDEX
};

class bufferNode
{
public:
	bufferNode();
	virtual ~bufferNode();

	void setBufferNode(int type, std::string filename, int offset);//type:table=0,index=1
	void flush();
	bool isMatch(int type, std::string filename, int offset);
	bool isEmpty();
	int age;
	bool pin;
	template<class T>
	void writeBlock(T data, int length, int offset)
	{
		this->dirty = true;
		memcpy(this->dataField + offset, (char*)&data, length);
	}
	void writeBlock(std::string data, int length, int offset);
	template<class T>
	void readBlock(T& dest, int length, int offset)
	{
		memcpy((char*)&dest, this->dataField + offset, length);
	}
	void readBlock(std::string& dest, int length, int offset);
protected:
	char* dataField;
	std::string fileName;
	int offset;
	int type;
	bool dirty;

};

class bufferManager
{
protected:

	std::vector<bufferNode>* bufferPool;
	int curAge;//for LRU
	int minAge;
	bool lruBase;
	std::vector<bufferNode>::iterator minIndex;
public:
	bufferManager();
	virtual ~bufferManager();
	std::vector<bufferNode>::iterator LRU();
	void flushAll();
	void usingBlock(std::vector<bufferNode>::iterator bn_ite);
	bufferNode& getBlock(int type, std::string filename, int offset);
};

#endif