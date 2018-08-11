test: test.o msg.pb-c.o
	gcc -Wall -g -O2 -o test test.c msg.pb-c.c -lprotobuf-c
clean:
	rm -f test *.o
#test: dulist.o dulist.pb-c.o
#	gcc -Wall -g -O2 -o dulist dulist.c dulist.pb-c.c -lprotobuf-c -lprotobuf
