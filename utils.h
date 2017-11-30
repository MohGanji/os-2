#ifndef __UTILS__
#define __UTILS__


#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <csignal>
#include <dirent.h> 
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// buffer[i] = '\0'; ==> changes buffer size, now we can only read i bytes.

// file descriptors for stdin, stdout and stderr
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// enum of states for client and data_server
enum STATE {Idle, Id};


// structures.
struct Packet {
    char *data;
    int len;
};


// utility functions

void print(std::string string);
void printbuff(char * str);
void printl(char* string, int size);
void printInt(int num);
struct Packet myread(int fd, char *buffer, int len);
int mystoi(char *str);
void *get_in_addr(struct sockaddr *sa);
int mysend(int sock, char *buf, int len);
struct Packet myrecv(int fd, char *buffer, int len);    
std::string accumulate(std::string base_dir, std::string id);
std::string add(std::string a, std::string b);
ssize_t sock_fd_write(int sock, char *buf, ssize_t buflen, int fd);
ssize_t sock_fd_read(int sock, char *buf, ssize_t bufsize, int *fd);
std::string readfile(std::string dir, std::string id);

#endif