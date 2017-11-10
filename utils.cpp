#include "utils.h"

using namespace std;

void print(string string){
    cerr << string;
    // write(STDOUT, string, strlen(string) );
    // write(STDOUT, "\n", 1);
}

void printbuff(char * str){
    cout << str;
}

void printl(char* string, int size){
    write(STDOUT, string, size);
}

void printInt(int num){
    char str[100], tmpstr[100];
    int itmp = 0, i = 0;
    int rem;
    while(num > 0){
        rem = num%10;
        num /=10;
        str[i] = '0' + rem;
        i++;
    }
    i--;
    for(i; i>=0 ; i--){
        tmpstr[itmp] = str[i];
        itmp++;
    }
    tmpstr[itmp] = '\0';
    print(tmpstr);
    return;
}

struct Packet myread(int fd, char *buffer, int len){
    struct Packet pack;
    int pack_len;
    pack_len = read(fd, buffer, len);
    pack.len = pack_len-1;
    buffer[pack_len-1] = '\0';
    pack.data = buffer;
    // print("in myread: ");print(pack.data);print("\n");
    // printInt(pack.len);print("\n\n");
    return pack;
}

int mystoi(char * str){
    int i, res = 0, len = strlen(str);
    for(i = 0; i < len; i++){
        if(i)
            res *= 10;
        res += str[i]-'0';
    }
    return res;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int mysend(int sock, char *buf, int len){
    if (send(sock, buf, len, 0) == -1) {
        print("ERR: send\n");
        return 1;
    }
    return 0;
}

struct Packet myrecv(int fd, char *buffer, int len){
    struct Packet pack;
    int pack_len;
    pack_len = recv(fd, buffer, len, 0);
    pack.len = pack_len;
    buffer[pack_len] = '\0';
    pack.data = buffer;
    // print("in myread: ");print(pack.data);print("\n");
    // printInt(pack.len);print("\n\n");
    return pack;
}