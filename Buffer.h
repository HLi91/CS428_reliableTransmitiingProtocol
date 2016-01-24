/*This in class project is written by
*Hongchuan Li in UIUC, CS428
*students who are taking cs428 in UIUC should not copying this file for their projects
*/

#ifndef BUFFER_H
#define BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sys/types.h>

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <thread>
#include <vector>
#include <mutex>

#include <time.h>
#include <chrono>
#include <functional>
#include <sys/types.h>
#include <algorithm> 

#define BUFAVAIL 0
#define BUFSENDED 1
#define BUFACKED 2
#define BUFUNAVAIL 3
#define PACKETSIZE 1400
#define SST 64


#define BDEBUG 0



using namespace std;
class sendBuffer
{
public:
	sendBuffer(char * data, unsigned long long int max);
	string getNext();
	void update(string ack);
	void add(char * data, unsigned long long int max);
	int rtt;
	void initial(char * data, unsigned long long int bytesToTransfer);
private:
	void startTimer();
	long getCurrentTime();
private:
	typedef struct bufElement
	{
		int time;
		unsigned char state;

	}bufElement_t;
	vector<bufElement_t> buffer;
	
	mutex bufMut, waitOne;
	double windowSize;
	int ssthresh;
	unsigned long long int BufStartPos, max, BufCUrPos;
	char* startPtr;

	thread* timerThread;

	
	


};

class RecBuffer
{
public:
	RecBuffer();
	string addNext(string packet);
	string getNext();
	void initial();
private:
	vector <string> buffer;
	unsigned long long int BufStartPos, BufCurPos;


	mutex bufMut, waitOne;
	bool emptyFlag = false;


};


#endif