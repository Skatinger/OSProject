#!/bin/bash
echo 'chosen password is' $1
yes $1 | ./../../build/connectionH &
echo 'press ctrl + c to interrupt server'
while :
do
    sleep 20s
done
