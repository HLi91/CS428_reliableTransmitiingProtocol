/*This in class project is written by
*Hongchuan Li in UIUC, CS428
*students who are taking cs428 in UIUC should not copying this file for their projects
*/
#include "TPManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>

#include <fstream>
#include <streambuf>
#include <mutex>

using namespace std;

int main(int argc, char *argv[])
{

	if (strstr(argv[1], "send"))
	{
		streampos size;
		char * memblock;

		TPManager* sendMNG = new TPManager();

		ifstream file(argv[2], ios::in | ios::binary | ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char[size];
			file.seekg(0, ios::beg);
			file.read(memblock, size);
			file.close();

			cout << "the entire file content is in memory";
			int port = 8080;
			sendMNG->sendTo(memblock, size, argv[3], 8080);
		}
	
	
	}
	else
	{
		
		TPManager* RecMNG = new TPManager();
		RecMNG->recvSetup(8080);
		
		string rec;
		while (1)
		{
			string ret = RecMNG->recv();
			if (ret.empty())
				break;
			rec += ret;
		}
		//cout << rec << endl;
		std::ofstream out("out");
		out << rec;
		out.close();
	
	}

}