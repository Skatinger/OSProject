#!/usr/bin/python
import os
import random
import signal
import string
import time
import subprocess
from typing import List, Any

####### input for test files ############
output_client_login = """

[32m[1mArt by Tua Xiong
                 \ __
--==/////////////[})))==*
                 / \ '          ,|
                    `\`\      //|                             ,|
                      \ `\  //,/'                           -~ |
   )             _-~~~\  |/ / |'|                       _-~  / ,
  ((            /' )   | \ / /'/                    _-~   _/_-~|
 (((            ;  /`  ' )/ /''                 _ -~     _-~ ,/'
 ) ))           `~~\   `\/'/|'           __--~~__--\ _-~  _/, 
((( ))            / ~~    \ /~      __--~~  --~~  __/~  _-~ /
 ((\~\           |    )   | '      /        __--~~  \-~~ _-~
    `\(\    __--(   _/    |'\     /     --~~   __--~' _-~ ~|
     (  ((~~   __-~        \~\   /     ___---~~  ~~\~~__--~ 
      ~~\~~~~~~   `\-~      \~\ /           __--~~~'~~/
                   ;\ __.-~  ~-/      ~~~~~__\__---~~ _..--._
                   ;;;;;;;;'  /      ---~~~/_.-----.-~  _.._ ~\     
                  ;;;;;;;'   /      ----~~/         `\,~    `\ \        
                  ;;;;'     (      ---~~/         `:::|       `\.      
                  |'  _      `----~~~~'      /      `:|        ()))),      
            ______/\/~    |                 /        /         (((((())  
          /~;;.____/;;'  /          ___.---(   `;;;/             )))'`))
         / //  _;______;'------~~~~~    |;;/\    /                ((   ( 
        //  \ \                        /  |  \;;,\                 `   
       (<_    \ \                    /',/-----'  _> 
        \_|     \_                 //~;~~~~~~~~~ 
                 \_|               (,~~   -Tua Xiong
                                    \~
                                     ~~[0m

[32m[1m```````````````````````````````````````````````
[0m[32m[1m`Welcome to the trivial client implementation!`
[0m[32m[1m```````````````````````````````````````````````
[0m
[32mLike this Greek mythological messenger, it will carry
[0m[32mwhat you have to say to the server!

[0m>> Trying to connect to 127.0.0.1
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

client_output_quit = """$ Logging out
Quitting
"""

client_output_get = """
$ Specify the key you want to get
The corresponding value is: 
    """
#####################################

root_password = "123"
clientcount = 1
put_length = 10
# number of key-value pairs inserted
N = 10

serverstart = "os.system(echo " + root_password + " | gnome-terminal -- ./connectionH.sh)"
rootstart = "os.system(echo " + root_password + " | gnome-terminal -- ./root.sh)"
client_writer_start = "./client_writer.sh "
client_reader_start = "./client_reader.sh "

# used to kill all child processes at the end
list_of_processes = []  # type: List[pid]


# generating root login file
# root creates clientcount number of users with usernames ranging from zero to clientcount
def generate_root_client_file():
    f = open("rootinput.txt", "w+")
    f.write("root\n" + root_password + "\n")
    # 2 * clientcount because first half is writers, second half is readers
    for i in range(clientcount * 2):
        f.write("addUser\n" + str(i) + "\n" + str(i) + "\n")
    f.close()


# generates a input file for testing for each client
def generate_client_files():
    for j in range(clientcount):
        h = j + clientcount
        # create package of writerX, readerX and expected_outputX for both
        writerX = open("input_writer" + str(j) + ".txt", "w+")
        readerX = open("input_reader" + str(h) + ".txt", "w+")
        expected_output_writerX = open("expected_output_writer" + str(j) + ".txt", "w+")
        expected_output_readerX = open("expected_output_reader" + str(h) + ".txt", "w+")

        # write login stuff to files
        writerX.write(str(j) + "\n" + str(j) + "\n")
        readerX.write(str(h) + "\n" + str(h) + "\n")
        expected_output_writerX.write(output_client_login)
        expected_output_readerX.write(output_client_login)

        # write put / get for clients, write expected output
        for i in range(N):
            rand1 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(put_length))
            rand2 = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(put_length))
            writerX.write("put\n" + rand1 + "\n" + rand2 + "\n")
            readerX.write("get\n" + rand1 + "\n")
            expected_output_writerX.write(client_output_put)
            expected_output_readerX.write(client_output_get + rand2)
        # write quitting stuff
        writerX.write("quit\n")
        readerX.write("quit\n")
        expected_output_writerX.write(client_output_quit)
        expected_output_readerX.write(client_output_quit)

        # close files
        writerX.close()
        readerX.close()
        expected_output_writerX.close()
        expected_output_readerX.close()


def init_base():
    pserver = subprocess.Popen(serverstart, shell=True)
    list_of_processes.append(pserver)
    proot = subprocess.Popen(rootstart, shell=True)
    list_of_processes.append(proot)


def init_tons_of_readers():
    for i in range(clientcount):
        p = subprocess.Popen(client_reader_start + str(i), shell=True)
        list_of_processes.append(p)


def init_tons_of_writers():
    for i in range(clientcount):
        p = subprocess.Popen(client_writer_start + str(i + clientcount), shell=True)
        list_of_processes.append(p)


# primary scripts
def test_put_get():
    init_base()
    init_tons_of_writers()
    init_tons_of_readers()
    validate()


def validate():
    if validate_writers():
        print("Writers Test successful")
    else:
        print("Writers Test unsuccessful")
    if validate_readers():
        print("Readers test successful")
    else:
        print("Readers test unsuccessful")


def validate_writers():
    if 'failed' in open("writer_exit.txt").read():
        return False
    return True


def validate_readers():
    if 'failed' in open("reader_exit.txt").read():
        return False
    return True


def cleanup():
    # kill all processes
    for process in list_of_processes:
        os.kill(process.pid, signal.SIGKILL)
    # remove temporary txt files
    os.system("rm *.txt")


# generate needed files
generate_client_files()
generate_root_client_file()

# start tests
test_put_get()

# clean up files
cleanup()
