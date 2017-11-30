#include "utils.h"
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 3) {
        print("ERROR, no port or directory provided!\n");
        exit(1);
    }
    char* port = (char *)argv[1];
    string base_dir = argv[2];
    main_server(port, base_dir);
    return 0;
}
