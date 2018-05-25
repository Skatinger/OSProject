#!/usr/bin/python

import random
import string
import time
import subprocess
from typing import List, Any

root_password = "123"
clientcount = 20

serverstart = "gnome-terminal -- ./connectionH.sh " + root_password
rootstart = "gnome-terminal -- ./root.sh " + root_password
client_writer_start = "gnome-terminal -- ./client_writer.sh "
client_reader_start = "gnome-terminal -- ./client_reader.sh"
#used to kill all child processes at the end
list_of_processes = []  # type: List[pid]


# generating root login file
def generate_root_client_file(root_password):
    f = open("rootinput.txt", "w+")
    f.write("root\n" + root_password + "\n")
    for i in range (clientcount):
        f.write("addUser\n" + str(i) + "\n" + str(i) + "\n")
    f.close()


def generate_client_files():
    f = open("client_writer_input.txt", "w+")
    p = open("client_reader_input.txt", "w+")
    for i in range(10000):
        rand1 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
        rand2 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
        f.write("put\n" + rand1 + "\n" + rand2 + "\n")
        p.write("get\n" + rand1 + "\n")
    f.write("quit")
    p.write("quit")
    f.close()
    p.close()


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


#generate needed files
generate_client_files()
generate_root_client_file(root_password)

#start tests
test_readers()
