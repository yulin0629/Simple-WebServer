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

    printf("AF_UNSPEC %d\n", AF_UNSPEC);
    printf("AF_INET %d\n", AF_INET);
    printf("hints.ai_family %d \n", hints.ai_family);
    
    struct addrinfo *pAddrInfo = pRes;
    do
    {
        printf("pRes->ai_family %d \n", pAddrInfo->ai_family);
    } while (pAddrInfo = pAddrInfo->ai_next);
    
    

    // Open socket
    int mainSocket;

    mainSocket = socket(pRes->ai_family, pRes->ai_socktype, 0);

    if (mainSocket < 0) {
        printf("Open main socket failed\n");
        return -1;
    }

    close(mainSocket);
    return 0;
}