all:
	gcc -o receiver receiver.c
	gcc -o reverse_tcp reverse_tcp.c -pthread

lol:
	gcc -o reverse_tcp_test reverse_tcp_test.c
