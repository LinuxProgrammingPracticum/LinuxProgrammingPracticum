all : server
server :
	gcc -g -o server -I/usr/include/mysql/ server.c -L/usr/lib64/mysql/ -lmysqlclient -lz
clean:
	rm server