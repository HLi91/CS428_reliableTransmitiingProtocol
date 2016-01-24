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
#include <algorithm> 

void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer);

int main(int argc, char** argv)
{
	unsigned short int udpPort;
	unsigned long long int numBytes;
	
	if(argc != 5)
	{
		fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
		exit(1);
	}
	udpPort = (unsigned short int)atoi(argv[2]);
	numBytes = atoll(argv[4]);
	
	reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
} 
void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer)
{
	streampos size;
	char * memblock;

	TPManager* sendMNG = new TPManager();

	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		/*
		size = bytesToTransfer;
		memblock = new char[bytesToTransfer];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "the entire file content is in memory";
		
		sendMNG->sendTo(memblock, size, hostname, hostUDPport);

		*/
		file.seekg(0, ios::beg);
		sendMNG->listen();
		unsigned long long int maxSize = 1024*1024*100;
		while (bytesToTransfer != 0)
		{
			unsigned long long int curSize = std::min(maxSize, bytesToTransfer);
			bytesToTransfer -= curSize;
			memblock = new char[curSize];
			file.read(memblock, curSize);
			

			cout << "the entire file content is in memory";

			sendMNG->sendTo(memblock, curSize, hostname, hostUDPport);
			delete memblock;

		}
		file.close();
	}


}