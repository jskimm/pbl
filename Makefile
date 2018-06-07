default: server.o client.o

server.o: server.c
	gcc -o server server.c -pthread -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient

client.o: client.c
	gcc -o client client.c -pthread

clean:
	rm server
	rm client
