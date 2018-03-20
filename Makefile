all: server.o client.o
	gcc -pthread -o server server.o
	gcc -o client client.o

client: client.o
	gcc -o client client.o
	
server: server.o server.h
	gcc -o server server.o

kvs: kvs.o kvs.h
	gcc -o kvs kvs.o kvs.h


#object files

client.o: client.c
	gcc -c client.c



#clear all object files
clear:
	rm *.o
	
