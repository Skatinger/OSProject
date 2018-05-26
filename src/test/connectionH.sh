#!/bin/bash
echo 'chosen root_password is' $1

echo $1 | ./../../build/connectionH &

echo 'press ctrl + c to interrupt server'
