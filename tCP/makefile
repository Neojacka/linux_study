.PHONY:all
all:server client
server:TCPServer.cc
	g++ -o $@ $^ -std=c++11 -pthread
client:TCPClient.cc
	g++ -o $@ $^ -std=c++11 -pthread
.PHONY:clean
clean:
	rm -rf server client