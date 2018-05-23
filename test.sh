build/ui 127.0.0.1 << EOF
root
1234
put key value
put anotherkey anothervalue
get key
put 1 1
put 2 2
put 3 3
put 7 7
delete 7
delete whatever
quit
EOF
