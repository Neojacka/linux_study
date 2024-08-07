.PHONY:all
all:main client
main:main.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
client:TCPClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
.PHONY:clean
clean:
	rm -f main client