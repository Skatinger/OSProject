# OSProject
Welcome, welcome, welcome! This is the repository for the Operating Systems lecture in the Summer Semester of 2018.
Authors: Group 6 aka Yvonne Eisenring, Alex Nyffenegger & Remo Geissbuehler.

## Dependencies
In order to run our project, it is imperative that *OpenSSL* be installed on your machine, as it is used in our project. It can be found on https://www.openssl.org. 
Mac users please note: macOS has a standard implementation of OpenSSL, this might not be the newest, though. As a consequence, it may happen that the compiler fetches the header file of a newer version, while using the old iplementation. Normally, this shouldn't happen as our makefile specifically tells the compiler where to search for the library.

## Other prerequisites
It is also imperative that the `crypto` folder be present in the project folder, which is not uploaded on github (since publishing private keys here would be beside the point). If you do not have one, contact us or generate its contents on your own.

## Compiling
In order to compile our project, simply move to the project folder and run `make`. The rest will be done for you.

## Cleaning up
Running `make clear` will delete all the binaries on your machine.

## Running the server
Once you have everything compiled, make sure to move to the project folder and run `./build/connectionH` to launch the server implementation. It will ask you to enter a password for the root account on the server, so enter something you can remember and hit enter.
The server should now be waiting for new connections!

Should you encounter `Unable to bind socket: Address already in use`, make sure there are no running `connectionH` or `ui` processes, as they will block the sockets.

## Running a client
In order to run the client implementation, cd to the project folder and run `./build/ui <ip-addr>`. This will try to set up a connection to a server with ip the given IP address. If you are running the server locally, just enter `./build/ui`. The interace will guide you through the application.

## Running the test script
Go to the /src/test folder and run `./testing.py`

## Exiting
The server has to be stopped with ctrl + C. The client, on the other hand, will exit using the `quit` command.
