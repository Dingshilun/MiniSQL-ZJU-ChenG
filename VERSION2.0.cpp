// BETAVERSION.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "interpreter.h"
#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
	cout << "Welcome to MINISQL!" << endl;
	Interpreter In;
	In.pipeline();
	return 0;
}

