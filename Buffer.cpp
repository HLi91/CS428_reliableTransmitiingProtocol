/*This in class project is written by
*Hongchuan Li in UIUC, CS428
*students who are taking cs428 in UIUC should not copying this file for their projects
*/

#include "Buffer.h"

sendBuffer::sendBuffer(char * data, unsigned long long int bytesToTransfer)
{
	startPtr = data;
	max = bytesToTransfer;
	BufStartPos = 0;
	windowSize = 1;
	bufElement_t elem;
	elem.time = 0;
	elem.state = BUFAVAIL;
	buffer.push_back(elem);
	BufCUrPos = 0;
	ssthresh = SST;
	rtt = 40;
	timerThread = new std::thread(&sendBuffer::startTimer, this);
}
void sendBuffer::initial(char * data, unsigned long long int bytesToTransfer)
{
	startPtr = data;
	max = bytesToTransfer;
	BufStartPos = 0;
	windowSize = 1;
	buffer.clear();
	bufElement_t elem;
	elem.time = 0;
	elem.state = BUFAVAIL;
	buffer.push_back(elem);
	BufCUrPos = 0;


}

string sendBuffer::getNext()
{
	if (BDEBUG)printf("sendBuffer::getNext enter\n");
	while (1)
	{
		bufMut.lock();

		int bufIndex = (BufCUrPos - BufStartPos) / PACKETSIZE;
		if (bufIndex < 0)
		{
			bufIndex = 0;
			BufCUrPos = BufStartPos;
		}
		if (bufIndex >= buffer.size())
		{
			//if (BDEBUG) cout << "BufCUrPos" << BufCUrPos << endl;
			//if (BDEBUG) cout << "BufStartPos" << BufStartPos << endl;
			//wait here
			waitOne.unlock();
			waitOne.lock();
			if (BDEBUG)printf("waiting...\n");
			bufMut.unlock();
			waitOne.lock();
			bufMut.lock();
			if (BDEBUG)printf("finish waiting...\n");
		}

		bufIndex = (BufCUrPos - BufStartPos) / PACKETSIZE;
		if (bufIndex < 0)
		{
			bufIndex = 0;
			BufCUrPos = BufStartPos;
		}
		
		string ret;
		if (bufIndex < buffer.size() && buffer[bufIndex].state == BUFAVAIL)
		{
			buffer[bufIndex].state = BUFSENDED;
			buffer[bufIndex].time = getCurrentTime();

			ret.append((char*)&BufCUrPos, sizeof(BufCUrPos));
			
			ret.append(startPtr + BufCUrPos, std::min(PACKETSIZE, (int)(max - BufCUrPos)));
			BufCUrPos += PACKETSIZE;

		}
		else
		{
			if (BufStartPos >= max)
			{
				bufMut.unlock();
				if (BDEBUG)printf("sendBuffer::getNext leave\n");
				return string();
			
			}
			else
			{
				//if (BDEBUG)printf("nearly finishn\n");
				bufMut.unlock();
				continue;
			}

		}
		//if (BDEBUG) cout << "BufCurPos " << BufCUrPos << endl;
		if (BDEBUG)cout << "send:packet No. " << BufCUrPos / PACKETSIZE - 1 << endl;
		bufMut.unlock();
		if (BDEBUG)printf("sendBuffer::getNext leave\n");
		return ret;
	}
	
	//if (BDEBUG)printf("sendBuffer::getNext leave\n");
}
void sendBuffer::update(string ack)
{
	//if (BDEBUG)printf("sendBuffer::update enter\n");
	if (ack.empty())
	{
		return;
	
	}
	bufMut.lock();

	unsigned long long int packetNum;
	memcpy(&packetNum, ack.c_str(), sizeof(packetNum));
	int bufIndex = (packetNum - BufStartPos) / PACKETSIZE;
	if (BDEBUG) cout << "recv ack No. " << packetNum/PACKETSIZE << endl;

	
	if (bufIndex < 0)
	{
		bufMut.unlock();
		//if (BDEBUG)printf("sendBuffer::update leave\n");
		return;
	
	}
	if (windowSize <= ssthresh)
	{									//slow start
		windowSize++;
		
		bufElement_t elem;
		elem.time = 0;
		if ((unsigned long long int)(windowSize-1) * (unsigned long long int)PACKETSIZE + BufStartPos >= max)
		{ 
			elem.state = BUFUNAVAIL;
		}
		else
		{
			elem.state = BUFAVAIL;
		}
		buffer.push_back(elem);
		//cout << "cur - max" << max - (unsigned long long int)(windowSize - 1) * (unsigned long long int)PACKETSIZE - BufStartPos << endl;
	
	}
	else
	{									//congestion aviodance
		windowSize += 1.0 / windowSize;
		bufElement_t elem;
		elem.time = 0;
		if ((unsigned long long int)(windowSize-1) * (unsigned long long int)PACKETSIZE + BufStartPos >= max)
		{
			elem.state = BUFUNAVAIL;
		}
		else
		{
			elem.state = BUFAVAIL;
		}
		//cout << "cur - max" << max - (unsigned long long int)(windowSize - 1) * (unsigned long long int)PACKETSIZE - BufStartPos << endl;
		if ((int)windowSize > buffer.size())
			buffer.push_back(elem);
	
	}

	for (int i = 0; i <= bufIndex; i++)
	{
		
		buffer.erase(buffer.begin());
		BufStartPos += PACKETSIZE;


		bufElement_t elem;
		elem.time = 0;
		if ((unsigned long long int)(windowSize -1) * (unsigned long long int)PACKETSIZE + BufStartPos >= max)
		{
			elem.state = BUFUNAVAIL;
		}
		else
		{
			elem.state = BUFAVAIL;
		}
		//cout << "cur - max" << max - (unsigned long long int)(windowSize - 1) * (unsigned long long int)PACKETSIZE - BufStartPos  << endl;
		buffer.push_back(elem);
	}
	if (BDEBUG) cout << "buffersize " << buffer.size() << endl;
	waitOne.unlock();
	bufMut.unlock();
	//if (BDEBUG)printf("sendBuffer::update leave\n");

}
void sendBuffer::startTimer()
{
	//if (BDEBUG)printf("sendBuffer::startTimer enter\n");
	while (true)
	{
		//return;
		bufMut.lock();
		int curTime = getCurrentTime();
		for (auto& it : buffer)										//todo: No need to go through the entire buffer, just the first one
		{
			if (it.state == BUFSENDED)
			{
				if (curTime - it.time > 2 * rtt)
				{
					if (BDEBUG) cout << "clear buffer" << endl;
					ssthresh = windowSize *0.667;
					
					windowSize = 1;
					BufCUrPos = BufStartPos;
					buffer.clear();
					bufElement_t elem;
					elem.time = 0;
					elem.state = BUFAVAIL;
					buffer.push_back(elem);
					waitOne.unlock();
					break;
				
				}
			
			}
			else
			{
			
				break;
			}
		
		}

		bufMut.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(rtt));			//todo: not wise to use rtt, instead use buffer[1].time - buffer[0].time
	}
	//if (BDEBUG)printf("sendBuffer::startTimer leave\n");
}


/*getCurrentTime
@return	time	long	the system time in ms(used as timestamp)
*/
long sendBuffer::getCurrentTime()
{
	//if (BDEBUG)printf("enter heartBeat::getCurrentTime\n");


	using namespace std::chrono;
	milliseconds ms = duration_cast< milliseconds >(
		system_clock::now().time_since_epoch()
		);

	//if (BDEBUG)printf("leave heartBeat::getCurrentTime\n");

	return ms.count();

}

















RecBuffer::RecBuffer()
{
	BufStartPos = 0;
	BufCurPos = 0;
}

void RecBuffer::initial()
{
	BufStartPos = 0;
	BufCurPos = 0;
	emptyFlag = false;
}

string RecBuffer::addNext(string packet)
{
	if (BDEBUG)printf("RecBuffer::addNext enter\n");
	bufMut.lock();
	while (emptyFlag)
	{
		bufMut.unlock();
		bufMut.lock();	
	
	}
	emptyFlag = packet.empty();
	if (emptyFlag)
	{
		waitOne.unlock();
		bufMut.unlock();
		if (BDEBUG)cout << "recv eof" << endl;
		
		return string();
	
	}

	unsigned long long int packetNum;
	memcpy(&packetNum, packet.c_str(), sizeof(packetNum));
	if (BDEBUG)cout << "Recv Packet No. " << packetNum / PACKETSIZE << endl;
	
	int bufIndex = (packetNum - BufStartPos)/PACKETSIZE;
	if (packetNum < BufCurPos )
	{
		unsigned long long int backAck = BufCurPos - PACKETSIZE;
		string ret = string((char*)&backAck, (char*)&backAck + sizeof(backAck));
		bufMut.unlock();
		//cout << "BufCurPos" << BufCurPos << endl;
		if (BDEBUG)printf("RecBuffer::addNext leave\n");
		return ret;
	}
	else
	{
		string content = packet.substr(sizeof(packetNum));
		if (bufIndex >= buffer.size())
		{
			buffer.resize(bufIndex + 1, string());
		}
		if (buffer[bufIndex].empty())
		{
			buffer[bufIndex] = content;
			
		}
		else
		{
			cout << "receive out of boundary duplicated msg" << endl;
		
		}
	
	}
	bufIndex = (BufCurPos - BufStartPos)/PACKETSIZE;
	
	while (bufIndex < buffer.size() && !buffer[bufIndex].empty())
	{
		BufCurPos += PACKETSIZE;
		bufIndex = (BufCurPos - BufStartPos) / PACKETSIZE;
	}
	unsigned long long int backAck = BufCurPos - PACKETSIZE;
	string ret = string((char*)&backAck, (char*)&backAck + sizeof(backAck));
	if (BufCurPos  > BufStartPos)
	{
		waitOne.unlock();	
	}
	bufMut.unlock();
	if (BDEBUG)cout << "Send ACK No. " << backAck / PACKETSIZE << endl;
	if (BDEBUG)printf("RecBuffer::addNext leave\n");
	return ret;
	
}
string RecBuffer::getNext()
{
	if (BDEBUG)printf("RecBuffer::getNext enter\n");
	bufMut.lock();
	
	if (BufCurPos <= BufStartPos)
	{
		if (emptyFlag)
		{
			initial();
			bufMut.unlock();
			return string();

		}
		waitOne.unlock();
		waitOne.lock();
		bufMut.unlock();
		waitOne.lock();
		bufMut.lock();
	
	}
	
	string ret;
	int bufIndex = (BufCurPos - BufStartPos) / PACKETSIZE;
	while (bufIndex > 0)
	{
	
		ret.append(buffer[0]);
		buffer.erase(buffer.begin());
		BufStartPos += PACKETSIZE;
		bufIndex = (BufCurPos - BufStartPos) / PACKETSIZE;
	
	}
	bufMut.unlock();
	if (BDEBUG)printf("RecBuffer::getNext leave\n");
	return ret;
	
}