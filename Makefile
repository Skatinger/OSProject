all: server.o client.o
	gcc -pthread -o server server.o
	gcc -o client client.o

client: client.o
	gcc -o client client.o
	
server: server.o server.h
	gcc -o server server.o

kvs: kvs.o keyvalue.h
	gcc -o kvs kvs.o keyvalue.h

kvs_test: kvs_test.o keyvalue.h
	gcc -o kvs_test kvs.o keyvalue.h


#object files

client.o: client.c
	gcc -c client.c

kvs_test.o: kvs_test.c
	gcc -c kvs_test.c


#clear all object files
clear:
	rm *.o
	
