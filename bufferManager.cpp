#include "stdafx.h"
#include "bufferManager.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
bufferNode::bufferNode()
{
	dataField = NULL;
	offset = NULL;
	dirty = false;
	pin = false;
	type = TABLE;
	age = NULL;
}

bufferNode::bufferNode(const bufferNode& bn)
{
	dataField = new char[BLOCK_SIZE];
	memcpy(dataField, bn.dataField, BLOCK_SIZE);
	offset = bn.offset;
	dirty = bn.dirty;
	type = bn.type;
	fileName = bn.fileName;
	pin = bn.pin;
	age = bn.age;
}

bufferNode::~bufferNode()
{
	delete[] dataField;
}

bufferManager::bufferManager()
{
	this->lruBase = false;
	this->curAge = 0;
	this->bufferPool = new bufferNode[MAX_BLOCK];
}


bufferManager::~bufferManager()
{
	flushAll();
	delete[] bufferPool;
}

void bufferNode::setBufferNode(int type, std::string filename, int offset)
{
	flush();
	dataField = new char[BLOCK_SIZE];
	cerr << "set bn " << hex << (int)dataField << endl;
	memset(dataField, 0, BLOCK_SIZE);
	this->fileName = filename;
	this->offset = offset;
	this->type = type;
	this->age = 0;
	dirty = false;
	pin = false;
	fstream diskFile;
	diskFile.open(this->fileName, ios::in | ios::out | ios::app | ios::binary);
	if (!diskFile.good())
		cerr << "file open error" << endl;
	diskFile.seekg(ios::beg + offset*BLOCK_SIZE);
	diskFile.read(dataField, BLOCK_SIZE);
	diskFile.close();
}


void bufferNode::flush()
{
	if (dirty)
	{
		fstream out;
		out.open(this->fileName, ios::out | ios::in | ios::binary);
		if (!out.good())
			cerr << "file open error" << endl;
		out.seekp(ios::beg + this->offset*BLOCK_SIZE);
		out.write(dataField, BLOCK_SIZE);
		out.close();
	}
}

bool bufferNode::isMatch(int type, std::string filename, int offset)
{
	return (this->type == type&&this->fileName == filename&&this->offset == offset);
}

bool bufferNode::isEmpty()
{
	return (this->dataField == NULL);
}


void bufferNode::writeBlock(string data, int length, int offset)
{
	this->dirty = true;
	const char* c = data.c_str();
	memcpy(this->dataField + offset, c, data.size());
	memset(this->dataField + offset + data.size(), 0, length - data.size());
}



void bufferNode::readBlock(string& dest, int length, int offset)
{
	char* c = new char[length + 1];
	memcpy(c, this->dataField + offset, length);
	c[length] = '\0';
	dest = c;
	delete[] c;
}

void bufferManager::flushAll()
{
	for (int i = 0; i<MAX_BLOCK; ++i)
	{
		bufferPool[i].flush();
	}
}


void bufferManager::usingBlock(int i)
{
	this->bufferPool[i].age = ++this->curAge;
}

int bufferManager::LRU()
{
	cerr << "LRU" << this->minAge << endl;
	return this->minIndex;
}

bufferNode& bufferManager::getBlock(int type, std::string filename, int offset)
{
	this->lruBase = false;

	for (int i = 0; i < BLOCK_SIZE; ++i)
	{
		if (bufferPool[i].isEmpty())
		{
			bufferPool[i].setBufferNode(type, filename, offset);
			usingBlock(i);
			return bufferPool[i];
		}
		if (!this->lruBase&&!bufferPool[i].pin)
		{//find first not pinned 
			minAge = bufferPool[i].age;
			minIndex = i;
			this->lruBase = true;
		}
		if (!bufferPool[i].pin&&bufferPool[i].age < minAge)
		{
			minAge = bufferPool[i].age;
			minIndex = i;
		}
		if (bufferPool[i].isMatch(type, filename, offset))
		{
			usingBlock(i);
			return bufferPool[i];
		}
	}
	//full
	int i = LRU();
	bufferPool[i].setBufferNode(type, filename, offset);
	usingBlock(i);
	return bufferPool[i];
}

bufferNode* bufferManager::getBlockPointer(int type, std::string filename, int offset)
{
	return &getBlock(type, filename, offset);
}
