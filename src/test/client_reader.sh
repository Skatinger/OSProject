#!/bin/bash

#opens a client and reads everything from txt file, then quits
#TODO compare server response with result
cat client_writer_input.txt | ./../../build/ui 127.0.0.1

