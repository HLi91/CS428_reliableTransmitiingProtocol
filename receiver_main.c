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
#include <iomanip>

void reliablyReceive(unsigned short int myUDPport, char* destinationFile);

int main(int argc, char** argv)
{
	unsigned short int udpPort;
	
	if(argc != 3)
	{
		fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);
		exit(1);
	}
	
	udpPort = (unsigned short int)atoi(argv[1]);
	
	reliablyReceive(udpPort, argv[2]);
}


long getCurrentTime()
{
	//if (BDEBUG)printf("enter heartBeat::getCurrentTime\n");


	using namespace std::chrono;
	milliseconds ms = duration_cast< milliseconds >(
		system_clock::now().time_since_epoch()
		);

	//if (BDEBUG)printf("leave heartBeat::getCurrentTime\n");

	return ms.count();

}

void reliablyReceive(unsigned short int myUDPport, char* destinationFile)
{
	
	TPManager* RecMNG = new TPManager();
	RecMNG->recvSetup(myUDPport);
	while (1)
	{
		

		std::ofstream ofs;
		ofs.open(destinationFile, std::ofstream::out | std::ofstream::app);
		int storedTime = getCurrentTime();
		while (1)
		{

			string ret = RecMNG->recv();
			if (ret.empty())
				break;
			ofs << ret;
			int curTime = getCurrentTime();
			int deltaTime = curTime - storedTime;
			storedTime = curTime;
			double speed = (double)ret.size() / 1024.0 / ((double)deltaTime / 1000.0) / 1024.0;

			std::cout << std::fixed;
			std::cout << std::setprecision(2);
			cout << "Speed = " << speed << "Mb/s                                        " << "\r" << flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
		ofs.close();
		

	}
}