CC = g++
CFLAG = -c
DEBUG = -g

ALL.O = utils.o main_server.o client_connect.o server.o client.o

all: $(ALL.O)
	$(CC) main_server.cpp utils.o server.o -o main_server.out
	$(CC) client.cpp utils.o client_connect.o -o client.out

utils.o: utils.cpp utils.h
	$(CC) $(CFLAG) utils.cpp

main_server.o: main_server.cpp utils.h server.o
	$(CC) $(CFLAG) main_server.cpp

server.o: server.cpp server.h utils.h utils.o
	$(CC) $(CFLAG) server.cpp

client.o: client.cpp client_connect.o utils.h utils.o
	$(CC) $(CFLAG) client.cpp

client_connect.o: client_connect.cpp client_connect.h utils.h utils.o
	$(CC) $(CFLAG) client_connect.cpp

clean:
	rm *.o *.out
