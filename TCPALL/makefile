.PHONY: all clean

all: TCPServer TCPClient

TCPServer: TCPServer.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread

TCPClient: TCPClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread

clean:
	rm -f TCPServer TCPClient
