#include "utils.h"

using namespace std;

int client_connect(char* ip, char* port, int *sock){
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        print("ERR: Socket creation\n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(mystoi(port));
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) 
    {
        print("ERR: invalid address!\n");
        return -1;
    }
  
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        print("ERR: Connection failed!\n");
        return -1;
    }
    return 0;
}