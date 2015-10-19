#ifndef __BUFFERMANAGER__H
#define __BUFFERMANAGER__H

#include <string>

class bufferNode
{
public:
	bufferNode();
	~bufferNode();

	void setBufferNode(int type, std::string filename, int offset);
	void flush();
	bool isMatch(int type, std::string filename, int offset);
	bool isEmpty();
protected:
	std::string fileName;
	int offset;
	bool dirty;
	bool pin;
};

class bufferManager
{
public:
	bufferManager();
	~bufferManager();

	bufferNode getBlock(int type, std::string filename, int offset);
};

#endif