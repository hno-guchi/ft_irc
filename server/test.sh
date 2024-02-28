#!/bin/sh

c++ -Wall -Wextra -Werror -std=c++98 -pedantic-errors Server.cpp ../utils/fatalError.cpp -o server
./server&

server_pid=$!

for i in {0..4}
do
	sleep 1
	curl -v telnet://127.0.0.1:8080 2>> result &
done

cat result
lsof -i:8080

sleep 1
kill $server_pid
rm result server
