/*This in class project is written by
*Hongchuan Li in UIUC, CS428
*students who are taking cs428 in UIUC should not copying this file for their projects
*/
#ifndef TPMANAGER_H
#define TPMANAGER_H
#include "Buffer.h"
#include "UdpWrapper.h"

#define TPDEBUG 0

using namespace std;

class TPManager
{
public:
	TPManager();
	void sendTo(char* data, unsigned long long int max, char* hostname, unsigned short int hostUDPport);
	int recvSetup(unsigned short int hostUDPport);
	string recv();
	~TPManager();
	void initialRec();
	void initialSend(char* data, unsigned long long int max);
	int listen();

private:
	sendBuffer* sender;
	RecBuffer* receiver;
	udpWrapper* udpSend, *udpRec;

	int recCbACK(udpInfo & info);
	int recCbMSG(udpInfo & info);
	

	volatile bool finish = false;

	int port = 8888;
};


#endif