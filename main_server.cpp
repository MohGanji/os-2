#include "utils.h"
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 2) {
        print("ERROR, no port provided!\n");
        exit(1);
    }
    char* port = (char *)argv[1];
    main_server(port);
    return 0;
}
