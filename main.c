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
    
    struct sockaddr_in client;
    socklen_t len;
    len = sizeof(client);
    int connect_socket;
    int n;
    char inbuf[2048];
    char buf[2048];
    while(1){
        connect_socket = accept(mainSocket, (struct sockaddr *)&client, &len);
        if (connect_socket < 0) {
            printf("accept\n");
            break;
        }

        n = read(connect_socket, inbuf, sizeof(inbuf));
        write(fileno(stdout), inbuf, n);
        if (n > 0) {
           
            FILE *f = fopen("index.html", "rb");
            int fLength;
            if (f) {
                 memset(buf, 0, sizeof(buf));
                 fLength = fread(buf, 1, sizeof(buf), f);
                 fclose(f);
            }
            
            write(connect_socket, buf, sizeof(buf));
        }

        close(connect_socket);        
    }
    

    close(mainSocket);
    return 0;
}