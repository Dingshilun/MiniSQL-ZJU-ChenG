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
}

bufferNode::~bufferNode()
{
	delete[] dataField;
}

bufferManager::bufferManager()
{
	this->lruBase = false;
	this->curAge = 0;
	this->bufferPool = new vector<bufferNode>(MAX_BLOCK);
}


bufferManager::~bufferManager()
{
	flushAll();
}

void bufferNode::setBufferNode(int type, std::string filename, int offset)
{
	flush();
	dataField = new char[BLOCK_SIZE];
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
	diskFile.seekg(ios::beg+offset*BLOCK_SIZE);
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
		out.seekp(ios::beg+this->offset*BLOCK_SIZE);
		/*
		for (int i = 0; i < sizeof(dataField); ++i)
		{
			cerr << hex << (int)dataField[i];
		}
		*/
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
	memcpy(this->dataField + offset, c, length);
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
	vector<bufferNode>::iterator bn_ite;
	for (bn_ite = this->bufferPool->begin(); bn_ite != this->bufferPool->end(); ++bn_ite)
	{
		bn_ite->flush();
	}
}

void bufferManager::usingBlock(std::vector<bufferNode>::iterator bn_ite)
{
	bn_ite->age = ++this->curAge;
}

std::vector<bufferNode>::iterator bufferManager::LRU()
{
	cerr << "LRU" << this->minAge << endl;
	return this->minIndex;
}

bufferNode& bufferManager::getBlock(int type, std::string filename, int offset)
{
	this->lruBase = false;
	vector<bufferNode>::iterator bn_ite;
	for (bn_ite = this->bufferPool->begin(); bn_ite != this->bufferPool->end(); ++bn_ite)
	{//sequential
		if (bn_ite->isEmpty())
		{
			bn_ite->setBufferNode(type, filename, offset);
			usingBlock(bn_ite);
			return *bn_ite;
		}
		if (!this->lruBase&&!bn_ite->pin)
		{//find first not pinned 
			minAge = bn_ite->age;
			minIndex = bn_ite;
			this->lruBase = true;
		}
		if (!bn_ite->pin&&bn_ite->age < minAge)
		{
			minAge = bn_ite->age;
			minIndex = bn_ite;
		}
		if (bn_ite->isMatch(type, filename, offset))
		{
			usingBlock(bn_ite);
			return *bn_ite;
		}
	}
	//full
	bn_ite = LRU();
	bn_ite->setBufferNode(type, filename, offset);
	usingBlock(bn_ite);
	return *bn_ite;
}
