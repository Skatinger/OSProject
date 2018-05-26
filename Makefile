all: dir build/client_test build/auth_test build/connectionH src/project.h build/ui

dir:
	mkdir -p -v build/obj


# attention: this is machine-dependant (depends on openssl installation)
build/connectionH: src/server/connection_handler.c src/server/server.c src/server/server.h src/utils/logger.c build/obj/key_value_v3.o src/server/access_handler.c src/utils/string_stuff.c src/server/authentication.c
	gcc -o build/connectionH src/server/authentication.c src/utils/string_stuff.c src/utils/logger.c build/obj/key_value_v3.o src/server/access_handler.c src/server/connection_handler.c -lssl -lcrypto src/server/server.c -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -pthread

build/auth_test: src/server/authentication.c src/test/auth_test.c src/server/authentication.h src/utils/logger.c src/server/kvs.c
	gcc -o build/auth_test src/utils/string_stuff.c src/utils/logger.c src/test/auth_test.c src/server/authentication.c src/server/kvs.c -lssl -lcrypto

build/client_test: src/client/client.c src/test/client_test.c src/utils/logger.c src/utils/string_stuff.c
	gcc src/test/client_test.c src/utils/string_stuff.c src/utils/logger.c src/client/client.c -o build/client_test -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -pthread

build/ui: src/client/client.c src/client/ui.c src/client/ui.h src/utils/logger.c
	gcc src/client/client.c -lssl -lcrypto src/client/ui.c src/utils/logger.c src/utils/string_stuff.c -o build/ui


# object files

build/obj/kvs_test.o: src/test/kvs_test.c
	gcc -pthread -c src/test/kvs_test.c -o build/obj/kvs_test.o

build/obj/key_value_v3.o: src/server/key_value_v3.c
	gcc -pthread -c src/server/key_value_v3.c -o build/obj/key_value_v3.o

build/obj/server.o: src/server/server.c
	gcc -pthread -c src/server/server.c -o build/obj/server.o -lssl

build/obj/auth_test.o: src/test/auth_test.c
	gcc -pthread -c src/test/auth_test.c -o build/obj/auth_test.o

build/obj/authentication.o: src/server/authentication.c
	gcc -pthread -o build/obj/authentication.o -c src/server/authentication.c

#clear all bin files
clear:
	rm -r build
