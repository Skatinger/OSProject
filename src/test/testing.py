#!/usr/bin/python
import os
import random
import signal
import string
import time
import subprocess

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
# number of testing clients
clientcount = 5
# length of keys and values inserted into key-value store
put_length = 10
# number of key-value pairs inserted
N = 10

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
    f.write("quit")
    f.close()


# generates a input file for testing for each client
def generate_client_files():
    server_file = open("con.txt", "w+")
    server_file.write("123")
    server_file.close()
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
    pserver = subprocess.Popen("./connectionH.sh")
    time.sleep(0.1)
    list_of_processes.append(pserver.pid)
    proot = subprocess.Popen("./root.sh")
    list_of_processes.append(proot.pid)


def init_tons_of_readers():
    for i in range(clientcount):
        start = client_reader_start + str(i + clientcount)
        p = subprocess.Popen(start, shell=True)
        list_of_processes.append(p.pid)


def init_tons_of_writers():
    for i in range(clientcount):
        start = client_writer_start + str(i)
        p = subprocess.Popen(start, shell=True)
        list_of_processes.append(p.pid)


# primary scripts
def test_put_get():
    init_base()
    print("waiting for base to init")
    time.sleep(2)
    init_tons_of_writers()
    init_tons_of_readers()
    #validate()


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
    print("opening writer_exit.txt")
    if 'failed' in open("writer_exit.txt").read():
        return False
    return True


def validate_readers():
    if 'failed' in open("reader_exit.txt").read():
        return False
    return True


def cleanup():
    # kill all possibly still running processes
    print("closing the following proccesses:")
    for process in list_of_processes:
        print(process)
    for i in list_of_processes:
        os.kill(i, signal.SIGINT)
    # remove temporary txt files
    os.system("rm *.txt")
    # kills all leftovers of connectionH
    os.system("kill $(ps aux | grep '[c]onnectionH' | awk '{print $2}')")


# generate needed files and compile
os.system("./compile.sh")
generate_client_files()
generate_root_client_file()

# start server, add users, init clients and validate their output
test_put_get()

# wait for all testing to finish
time.sleep(clientcount / 4)

# clean up files
cleanup()
