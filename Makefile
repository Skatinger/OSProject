# Variable declarations used in this wonderful makefile
#SRC=src
#CLIENT_SRC=${SRC}/client
#SERVER_SRC=${SRC}/server
#TEST_SRC=${SRC}/test

#CFLAGS=-pthread

#BUILDDIR=build
#OBJDIR=build/obj

#CC=gcc

#sources := $(wildcard $(SRC)/*/*.c)
#objects := $(patsubst $(SRC)/%, $(OBJDIR)/%, $(patsubst %.c, %.o,$(sources)))
#headerDeps := $(wildcard $(SRC)/*/*.h) # so that stuff is recompliled if a header changes
#execs = $(BUILDDIR)/client $(BUILDDIR)/server $(BUILDDIR)/kvs_test

#all : $(objects) $(execs) $(headerDeps)

# compile all object files
#$(objects):
#	touch $@
	#$(CC) $(CFLAGS) -o $@ -c $*.c

#$(OBJDIR)/*.o : $(SRC)/*.c
#	$(CC) $(CFLAGS) -o $@ -c $<


#all: build/obj/build/obj/server.o build/obj/client.o
#	gcc -pthread -o server build/obj/server.o
#	gcc -o client build/obj/client.o

#$(BUILDDIR)/client: $(OBJDIR)/client/*
	#gcc -o $@ $^

#$(BUILDDIR)/server: $(OBJDIR)/server/*
	#gcc -o $@ $^

#$(BUILDDIR)/kvs: $(OBJDIR)/server/kvs.o
#	gcc -o $@ $<

#$(BUILDDIR)/kvs_test: $(OBJDIR)/server/kvs*
	#gcc -o $@ $^


all: build/client  build/kvs_test build/auth_test build/connectionH build/sslTest src/project.h #build/kvs

build/client: src/client/client.c
	gcc -o build/client src/client/client.c -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -pthread

#build/server: build/obj/server.o src/server/server.h
#	gcc -o build/server -lssl -lcrypto build/obj/server.o

# attention: this is machine-dependant (depends on openssl installation)
build/connectionH: src/server/connectionHandler.c src/server/server.c src/server/server.h
	gcc -o build/connectionH src/server/connectionHandler.c -lssl -lcrypto src/server/server.c -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -pthread

#build/kvs: build/obj/kvs.o src/server/keyvalue.h
#	gcc -o build/kvs build/obj/kvs.o

build/kvs_test: build/obj/kvs_test.o src/server/keyvalue.h build/obj/kvs.o
	gcc -o build/kvs_test build/obj/kvs.o build/obj/kvs_test.o

build/auth_test: build/obj/authentification.o build/obj/auth_test.o src/server/authentification.h
	gcc -o build/auth_test  build/obj/auth_test.o build/obj/authentification.o -lnettle

# attention: this is machine-dependant (depends on openssl installation)
build/sslTest: src/server/sslTest.c
	gcc src/server/sslTest.c -o build/sslTest  -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include

#object files

build/obj/client.o: src/client/client.c
	gcc -pthread -c src/client/client.c -o build/obj/client.o

build/obj/kvs_test.o: src/test/kvs_test.c
	gcc -pthread -c src/test/kvs_test.c -o build/obj/kvs_test.o

build/obj/kvs.o: src/server/kvs.c
	gcc -pthread -c src/server/kvs.c -o build/obj/kvs.o

build/obj/server.o: src/server/server.c
	gcc -pthread -c src/server/server.c -o build/obj/server.o -lssl

build/obj/auth_test.o: src/test/auth_test.c
	gcc -pthread -c src/test/auth_test.c -o build/obj/auth_test.o

build/obj/authentification.o: src/server/authentification.c
	gcc -pthread -o build/obj/authentification.o -c src/server/authentification.c
#clear all object files
clear:
	rm *.o
