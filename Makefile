OPTION=-fsanitize=undefined -fsanitize=address

default: server.o client.o

server.o: server.c
	gcc -o server src/server.c -pthread -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient 

client.o: client.c
	gcc -o client src/client.c -pthread $(OPTION)

clean:
	rm server
	rm client
