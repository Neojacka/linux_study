path=-I ../reactor/

.PHONY:all
all:kvstore client test
kvstore:kvstore.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp $(path) -DDEBUG -pthread
client:TCPClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp $(path)  -pthread
test:test.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp $(path)  -pthread
.PHONY:clean
clean:
	rm -f kvstore client test