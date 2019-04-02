#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

char portNumber[] = "80";

int main(void) {
    
    struct addrinfo *pRes, hints;
    int err;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    err = getaddrinfo(NULL, portNumber, &hints, &pRes);

    if (err != 0) {
        printf("getaddrinfo : %s\n", gai_strerror(err));
        return 1;
    }
    
    // Open socket
    int mainSocket;

    mainSocket = socket(pRes->ai_family, pRes->ai_socktype, 0);

    if (mainSocket < 0) {
        printf("Open main socket failed\n");
        return -1;
    }

    if (bind(mainSocket, pRes->ai_addr, pRes->ai_addrlen) != 0) {
        printf("Bind error! Port can't not be binded.\n");
        return -1;
    }

    freeaddrinfo(pRes);
    
    if (listen(mainSocket, 5) != 0) {
        printf("listen port Error\n");
        return 1;
    }
    

    close(mainSocket);
    return 0;
}