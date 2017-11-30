#include "utils.h"
#include "client_connect.h"  

using namespace std;

int main(int argc, char *argv[])
{
    int sock = 0;
    char buffer[1024] = {0};
    char* ip, *port;
    int status = 0;
    int response_len;
    struct Packet packet;    

    string id;

    if(argc < 3){
        print("ERR: no ip or port provided!\n");
        exit(1);
    }
    ip =  (char *) argv[1];
    port = (char *) argv[2];
    status = client_connect(ip, port, &sock);
    if(status != 0){
        print("ERR: failed to connect to server!\n");
        return -1;
    }
    send(sock, "__CLIENT__", 10, 0);
    packet = myrecv(sock, buffer, 1024);
    cout << packet.data << endl;
    cout << "Please enter your car id: ";
    while(packet.len > 0){
        cin >> id;
        send(sock, id.c_str(), id.length(), 0);
        packet = myrecv(sock, buffer, 1024);
        cout << packet.data << endl;
        if(strncmp(packet.data, "fee", 3) == 0 )
            break;
    }

    return 0;
}

