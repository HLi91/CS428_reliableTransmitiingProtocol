/*This in class project is written by
*Hongchuan Li in UIUC, CS428
*students who are taking cs428 in UIUC should not copying this file for their projects
*/
#include "TPManager.h"

TPManager::TPManager()
{
	udpSend = new udpWrapper();
	udpRec = new udpWrapper();
}


int TPManager::listen()
{
	while (udpRec->udpRecSetup(to_string(port).c_str()) < 0)
	{
		port++;
	}
	using namespace std::placeholders;
	udpRec->udpRecAsyn(bind(&TPManager::recCbACK, this, _1), 0, NULL);


}
void TPManager::sendTo(char* data, unsigned long long int max, char* hostname, unsigned short int hostUDPport)
{
	if (TPDEBUG)printf("TPManager::sendTo enter\n");
	sender = new sendBuffer(data, max);
	
	

	while (1)
	{
		string out;
		out.append(to_string(port));

		out += "\r\n";
		string msg = sender->getNext();
		out += msg;
		
		udpSend->udpSend(out, string(hostname), to_string(hostUDPport));
		if (msg.size() == 0)
		{
			int count = 0;
			while (!finish && count <= 3)
			{
				udpSend->udpSend(out, string(hostname), to_string(hostUDPport));
				std::this_thread::sleep_for(std::chrono::milliseconds(sender->rtt));
				count++;
			}
			delete sender;
			return;
		}
	}
}


string TPManager::recv()
{
	return receiver->getNext();

}


int TPManager::recCbACK(udpInfo & info)
{
	if (TPDEBUG)printf("TPManager::recCbACK enter\n");
	if (info.msg.empty())
	{
		finish = true;
	
	}
	sender->update(info.msg);
	if (TPDEBUG)printf("TPManager::recCbACK leave\n");
}





int TPManager::recvSetup(unsigned short int hostUDPport)
{
	if (TPDEBUG)printf("TPManager::recvSetup enter\n");
	if (udpRec->udpRecSetup(to_string(hostUDPport).c_str()) < 0)
		return -1;
	using namespace std::placeholders;
	udpRec->udpRecAsyn(bind(&TPManager::recCbMSG, this, _1), 0, NULL);
	receiver = new RecBuffer();
	if (TPDEBUG)printf("TPManager::recvSetup leave\n");
}

int TPManager::recCbMSG(udpInfo & info)
{
	if (TPDEBUG)printf("TPManager::recCbMSG enter\n");
	size_t pos = info.msg.find("\r\n");
	string retPort = info.msg.substr(0, pos);
	string ack = receiver->addNext(info.msg.substr(pos + 2));
	udpSend->udpSend(ack, info.ip, retPort);
	if (TPDEBUG)printf("TPManager::recCbMSG leave\n");

}
TPManager::~TPManager()
{
	udpRec->~udpWrapper();
	udpSend->~udpWrapper();
	delete sender;
	delete receiver;
}

void TPManager::initialRec()
{
	
	receiver->initial();

}

void TPManager::initialSend(char* data, unsigned long long int max)
{

	sender->initial(data, max);

}