#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

int main(void) {
    
    int mainSocket;

    mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (mainSocket < 0) {
        printf("Open main socket failed\n");
        return -1;
    }

    close(mainSocket);
    return 0;
}