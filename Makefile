all: server.o client.o
	gcc -pthread -o server server.o
	gcc -o client client.o

client: client.o
	gcc -o client client.o
	
server: server.o
	gcc -o server server.o
