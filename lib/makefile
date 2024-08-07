libmyath.so:my_add.o my_sub.o
	gcc -shared -o $@ $^
my_add.o:my_add.c
	gcc -fPIC -c my_add.c -o my_add.o
my_sub.o:my_sub.c
	gcc -fPIC -c my_sub.c -o my_sub.o


.PHONY:output
output:
	mkdir -p mylib/include
	mkdir -p mylib/lib 
	cp -f *.h mylib/include
	cp -f *.so mylib/lib 
	 
.PHONY:clean
clean:
	rm -f *.o libmath.so 
