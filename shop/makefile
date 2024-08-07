.PHONY:all
all:onshop shopcl
onshop:onlineshop.cpp
	g++ -o $@ $^ -std=c++11 -lpthread
shopcl:shopclient.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -rf onshop shopcl