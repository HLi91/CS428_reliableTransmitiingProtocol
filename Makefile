all: TPmanagerTest.o Buffer.o TPManager.o UdpWrapper.o receiver_main.o sender_main.o
	g++ -o output TPmanagerTest.o UdpWrapper.o Buffer.o TPManager.o  -pthread -g
	g++ -o reliable_sender sender_main.o UdpWrapper.o Buffer.o TPManager.o  -pthread -g
	g++ -o reliable_receiver receiver_main.o UdpWrapper.o Buffer.o TPManager.o  -pthread -g
	

TPmanagerTest.o: TPmanagerTest.cpp
	g++ -c -std=c++11 -pthread TPmanagerTest.cpp -g
Buffer.o: Buffer.cpp
	g++ -c -std=c++11 -pthread Buffer.cpp -g
	
	
TPManager.o: TPManager.cpp
	g++ -c -std=c++11 -pthread TPManager.cpp -g

UdpWrapper.o: UdpWrapper.cpp
	g++ -c -std=c++11 -pthread UdpWrapper.cpp -g
	
receiver_main.o: receiver_main.c
	g++ -c -std=c++11 -pthread receiver_main.c -g
	
sender_main.o: sender_main.c
	g++ -c -std=c++11 -pthread sender_main.c -g
	

	
clean:
	rm -f *o output reliable_sender reliable_receiver out







