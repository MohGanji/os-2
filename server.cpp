#include "utils.h"

using namespace std;

void main_server(char* PORT){
    
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    enum STATE state[100] = {Idle};
    char ds_ports[100][6];
    
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    
    char buf[1024];    // buffer for client data
    int buf_len;
    char *res_buf;

    char remoteIP[INET6_ADDRSTRLEN];
    
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct Packet packet;

    
    struct addrinfo hints, *ai, *p;
    
    
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    
    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    // fill this null input for ip.
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "ERR: getaddrinfo failed!" << endl;
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        
        break;
    }
    
    // if we got here, it means we didn't get bound
    if (p == NULL) {
        print("ERR: bind failed!\n");
        exit(2);
    }
    
    freeaddrinfo(ai); // all done with this
    
    // listen
    if (listen(listener, 10) == -1) {
        print("ERR: listen failed!\n");
        exit(3);
    }
    
    // add the listener to the master set
    FD_SET(listener, &master);
    // add stdin to master set
    FD_SET(STDIN, &master);
    
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    print("Server is Up!\n");    

    // main loop
    while(1) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            print("ERR: select failed!\n");
            exit(4);
        }
        
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    
                    
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);
                        
                        if (newfd == -1) {
                            print("ERR: failed to accept!\n");
                        } else {
                            FD_SET(newfd, &master); // add to master set
                            if (newfd > fdmax) {    // keep track of the max
                                fdmax = newfd;
                            }
                            print("New connection from: ");
                            print((char *)inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN));
                            print(" on socket ");printInt(newfd);print("\n");
                        }
                    } else {
                        // handle data from a client
                        packet = myread(i, buf, sizeof buf);

                        if (packet.len <= 0) {
                            // got error or connection closed by client
                            if (packet.len == 0) {
                                // connection closed
                                print("socket ");printInt(i);print(" hung up\n");
                            } else {
                                print("ERR: receive!\n");
                            }
                            close(i); // bye!
                            FD_CLR(i, &master); // remove from master set
                            state[i] = Idle;
                    } else {
                        if( i == STDIN && (strcmp(packet.data, "quit") == 0))
                                exit(0);
                        // we got some data from a client
                        //change the structure of recv, so that it returns a packet. like myread in client
                        print("log: new message: ");printl(packet.data, packet.len); print("\n");
                        if (FD_ISSET(i, &master)) {
                            switch (state[i]){
                                case Idle:
                                    if (strncmp(packet.data, "__CLIENT__", packet.len) == 0){
                                        print("log: A Client Connected!\n");
                                        state[i] = Idle;
                                        send(i, "welcome", 7, 0);
                                    }else{
                                        if( i != STDIN){ 
                                            print("log: Some unknown device connected!\n");
                                            send(i, "Who the hell are you??", 22, 0);
                                        }
                                    }
                                    break;
                                //case STATE...
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END while(1)--and you thought it would never end!    
}

