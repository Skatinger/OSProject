build/ui 127.0.0.1 << EOF
remo
remospw
put key value
put anotherkey anothervalue
get key
put 27 1
put 28 2
put 29 3
put 30 7
delete 7
delete 3
delete whatever
quit
EOF
