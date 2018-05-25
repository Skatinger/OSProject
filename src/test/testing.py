#!/usr/bin/python
import os
import random
import signal
import string
import time
import subprocess
from typing import List, Any

####### input for test files ############
client_output_login = """

```````````````````````````````````````````````
`Welcome to the trivial client implementation!`
```````````````````````````````````````````````


>> Trying to connect to 127.0.0.1
-- Created socket --
-- Joined socket with tls -- 
-- Connected to server -- 
Please provide your username:
Please provide your password
Thanks. Checking ...
Succesfully logged in!
Enter a command. 'help' will print help.
"""
client_output_put = """
$ Specify the key you want to add
key value accepted
And the value for key value
New key and value successfully set!
"""

client_output_quit = """
$ Quitting

"""

client_output_get = """
$ Specify the key you want to get
The corresponding value is: 
    """
#####################################

root_password = "123"
clientcount = 20
#number of key-value pairs inserted
N = 10

serverstart = "gnome-terminal -- ./connectionH.sh " + root_password
rootstart = "gnome-terminal -- ./root.sh " + root_password
client_writer_start = "gnome-terminal -- ./client_writer.sh "
client_reader_start = "gnome-terminal -- ./client_reader.sh "

#used to kill all child processes at the end
list_of_processes = []  # type: List[pid]


# generating root login file
#root creates clientcount number of users with usernames ranging from zero to clientcount
def generate_root_client_file():
    f = open("rootinput.txt", "w+")
    f.write("root\n" + root_password + "\n")
    for i in range (clientcount):
        f.write("addUser\n" + str(i) + "\n" + str(i) + "\n")
    f.close()


#generates a input file for testing for each client
def generate_client_files():
    for j in range(clientcount):
        #create files for each client
        client1 = open("client_writer_input" + str(j) + ".txt", "w+")
        client2 = open("client_reader_input" + str(j) + ".txt", "w+")
        diff1 = open("diff_writer" + str(j) + ".txt", "w+")
        diff2 = open("diff_reader" + str(j) + ".txt", "w+")

        #write login stuff to files
        client1.write(str(j) + "\n" + str(j) + "\n")
        client2.write(str(j) + "\n" + str(j) + "\n")
        diff1.write(client_output_login)
        diff2.write(client_output_login)

        #write put / get stuff
        for i in range(N):
            rand1 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
            rand2 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
            client1.write("put\n" + rand1 + "\n" + rand2 + "\n")
            client2.write("get\n" + rand1 + "\n")
            diff1.write(client_output_put)
            diff2.write(client_output_get + rand2)
        #write quitting stuff
        client1.write("quit\n")
        client2.write("quit\n")
        diff1.write(client_output_quit)
        diff2.write(client_output_quit)

        #close files
        client1.close()
        client2.close()
        diff1.close()
        diff2.close()


def init():
    pserver = subprocess.Popen(serverstart, shell=True)
    list_of_processes.append(pserver)
    proot = subprocess.Popen(rootstart, shell=True)
    list_of_processes.append(proot)


def tons_of_readers():
    for i in range(1):
        p = subprocess.Popen(client_reader_start + str(i), shell=True)
        list_of_processes.append(p)


def tons_of_writers():
    for i in range(1):
        p = subprocess.Popen(client_writer_start + str(i), shell=True)
        list_of_processes.append(p)


# primary scripts
def test_readers():
    init()
    tons_of_readers()


def test_writers():
    init()
    tons_of_writers()


def cleanup():
    #kill all processes
    for process in list_of_processes:
        os.kill(process.pid, signal.SIGKILL)
    os.system("rm *.txt")


#generate needed files
generate_client_files()
generate_root_client_file()

#start tests
#test_readers()

cleanup()