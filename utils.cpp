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
    pack.len = pack_len;
    buffer[pack_len] = '\0';
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
    if(fd == STDIN)
        pack_len = read(fd, buffer, len) - 1;
    else
        pack_len = recv(fd, buffer, len, 0);
    pack.len = pack_len;
    buffer[pack_len] = '\0';
    pack.data = buffer;
    // print("in myread: ");print(pack.data);print("\n");
    // printInt(pack.len);print("\n\n");
    return pack;
}

string readfile(string dir, string id){
    // found id, date, and fee    
    string fid, fdate, ffee;
    string sum = "0";
    
    ifstream fs (dir.c_str());
    if(fs.is_open()){
        while (!fs.eof()){
        fs >> fid >> fdate >> ffee;
        // cout << "line: " << fid << ": " << ffee << endl;
            if(fid == id)
                sum = add(ffee, sum);
        }
        fs.close();
        
        // cout << "sum in this file: " << sum << endl;
    }
    return sum;
}

int readdirectory(string base_dir,vector<string> &files, int &count){
    struct dirent *pDirent;
    DIR *pDir = opendir ((base_dir + "/").c_str());
    
    
    if(pDir == NULL){
        count = 1;
        return 0;
    }
    else {
        while ((pDirent = readdir(pDir)) != NULL) {
            if( strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0 ){
                string new_dir = pDirent->d_name;
                files.push_back(new_dir);
                count ++;
            }
        }
        closedir (pDir);
        return 1;
    }
    
}

string accumulate(string base_dir, string id){
    
    // Father code (before child processes start)
    pid_t child_pid, wpid;
    int status = 0;
    int count = 0;

    int len;
    
    string sum = "0";
    
    int filescount = 0;
    vector<string> files;
    int isdir = readdirectory(base_dir, files, filescount);
    int pipefds[filescount][2];
    int piperet[filescount];    
    
    // for(int i = 0; i < filescount; i++)
    //     cout << files[i] << endl;

    for(int i = 0 ;i < filescount ; i++){
        piperet[i] = pipe(pipefds[i]);
        if(fork() == 0){
            // calculate sum here and send it to parent
            
            if(isdir == 1){
                sum = accumulate(base_dir + "/" + files[i], id);
                // cout << "acc sum : " << sum << endl;
                piperet[i] = write(pipefds[i][1], sum.c_str(), sum.length());
            }
            else{ // its a file
                sum = readfile(base_dir, id);
                piperet[i] = write(pipefds[i][1], sum.c_str(), sum.length());
            }

            if(piperet[i] != sum.length()){
                cout << getpid() << " ";cerr << "pipe did not send the expected value!" << endl;
                cout << getpid() << " ";cerr << sum << " " << piperet[i] << endl;
                exit(2);
            } else if(piperet[i] == -1){
                printf("ERROR writing\n");
            }
            exit(0);
            // send sum to father
        }
    }

    for(int i =0 ; i < count ; i++)
        wait(NULL);
    // while ((wpid = wait(&status)) > 0); // this way, the father waits for all the child processes
    // Father code (After all child processes end)
    
    char buffer [128];
    memset(buffer, '\0', 128);
    sum = "0";
    for(int i = 0 ; i < filescount ; i++){

        piperet[i] = read(pipefds[i][0], buffer, 128 ); /* Read from pipe */
        
        if(piperet[i] == -1){
            printf("ERROR reading pipe\n");
        }
        if (piperet[i] != strlen(buffer)) {
            printf("Read did not return expected value\n");
            exit(4);
        }
        string tmpsum = buffer;
        memset(buffer, '\0', 128);
        sum = add(sum, tmpsum);
    }
    return sum;
}

string add(string a, string b){
    string res = to_string(stoi(a) + stoi(b));
    return res;
}


ssize_t sock_fd_write(int sock, char *buf, ssize_t buflen, int fd){
    ssize_t     size;
    struct msghdr   msg;
    struct iovec    iov;
    union {
        struct cmsghdr  cmsghdr;
        char        control[CMSG_SPACE(sizeof (int))];
    } cmsgu;
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        printf ("passing fd %d\n", fd);
        *((int *) CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        printf ("not passing fd\n");
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0)
        perror ("sendmsg");
    return size;
}

ssize_t sock_fd_read(int sock, char *buf, ssize_t bufsize, int *fd){
    ssize_t     size;

    if (fd) {
        struct msghdr   msg;
        struct iovec    iov;
        union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        } cmsgu;
        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg (sock, &msg, 0);
        if (size < 0) {
            perror ("recvmsg");
            exit(1);
        }
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET) {
                fprintf (stderr, "invalid cmsg_level %d\n",
                     cmsg->cmsg_level);
                exit(1);
            }
            if (cmsg->cmsg_type != SCM_RIGHTS) {
                fprintf (stderr, "invalid cmsg_type %d\n",
                     cmsg->cmsg_type);
                exit(1);
            }

            *fd = *((int *) CMSG_DATA(cmsg));
            printf ("received fd %d\n", *fd);
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0) {
            perror("read");
            exit(1);
        }
    }
    return size;
}