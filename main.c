#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

char portNumber[] = "80";

typedef struct _HTTPHeader {
  char *method;
  char *host;
  char *path;
} HTTPHeader;

int getHeaderLine(char **pointer, int lineNumber, char *inbuf, int size) {
  int lineStart = 0;
  int counter = 0;

  for(int index = 0; index < size; index++)
  {
    if (inbuf[index] == '\n') {
      if (lineNumber == counter) {
        int size = index - lineStart;
        *pointer = (char *)malloc(size);
        memcpy(*pointer, &inbuf[lineStart], size);
        (*pointer)[size-1] = '\0';
        return 0;
      }
      else {
        lineStart = index + 1;
        counter++;
        if(lineStart >= size) {
          return -1;
        }
      }
    }
  }
  
  return -2;
}

int readPageFile(char name[], char *buf, int bufferSize) {
  FILE *f = fopen(name, "rb");
  int fLength;
  if (f) {
    memset(buf, 0, bufferSize);
    fLength = fread(buf, 1, bufferSize, f);
    fclose(f);
  }

  return fLength;
}

int getPathFromHeader(char **path, char line[], int HTTPMethodLength) {
  int counter = HTTPMethodLength;
  while(line[counter] != '\0'){
    if (line[counter] == ' '){
      int size = counter - HTTPMethodLength + 1;
      *path = malloc(size);
      memcpy(*path, &(line[HTTPMethodLength]), size);
      (*path)[size-1] = '\0';
      return 0;
    }
    
    counter++;
  }
  
  return -1;
}

int getValueFromHeader(char **value, char line[], int startFrom) {
  int endedTo = startFrom;
  while(line[endedTo] != '\0'){
    endedTo++;
  }
  int size = endedTo - startFrom + 1;
  *value = malloc(size);
  memcpy(*value, &(line[startFrom]), size);
  (*value)[size-1] = '\0';
  return 0;
}

int bindSocketToPort(int *mainSocket, char portNumber[]) {
  struct addrinfo *pRes, hints;
  int err;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;

  err = getaddrinfo(NULL, portNumber, &hints, &pRes);

  if (err != 0) {
    printf("getaddrinfo : %s\n", gai_strerror(err));
    return -1;
  }

  *mainSocket = socket(pRes->ai_family, pRes->ai_socktype, 0);

  if (*mainSocket < 0) {
    printf("Open main socket failed\n");
    return -2;
  }

  int yes = 1;
  setsockopt(*mainSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

  if (bind(*mainSocket, pRes->ai_addr, pRes->ai_addrlen) != 0) {
    printf("Bind error! Port can't not be binded.\n");
    return -3;
  }

  freeaddrinfo(pRes);

  if (listen(*mainSocket, 5) != 0) {
    printf("listen port Error\n");
    return -4;
  }

  return 0;
}

void listening(int mainSocket) {
  int connect_socket;
  struct sockaddr_in client;
  socklen_t len;
  len = sizeof(client);
  
  int n;
  char inbuf[2048];
  char buf[2048];
  char buf404[2048];
  int pageFileSize = 0;

  readPageFile("404.html", buf404, sizeof(buf404));

  while (1) {
    connect_socket = accept(mainSocket, (struct sockaddr *)&client, &len);
    if (connect_socket < 0) {
      printf("accept\n");
      break;
    }

    n = read(connect_socket, inbuf, sizeof(inbuf));
    if (n > 0) {
      HTTPHeader head;
      head.method = "GET";

      char *line = NULL;
      int lineIndex = 0;
      while(getHeaderLine(&line, lineIndex, inbuf, n) == 0) {
        if (line) {
          if (strncmp(line, "GET", 3) == 0) {
            char *path = NULL;
            getPathFromHeader(&path, line, sizeof("GET"));
            head.path = path;
          }
          else if(strncmp(line, "Host:", 5) == 0) {
            char *host = NULL;
            getValueFromHeader(&host, line, sizeof("Host:"));
            head.host = host;
          }
          free(line);
          lineIndex++;
        }
        else {
          break;
        }
      }

      char fullpath[2048];
      sprintf(fullpath, "%s%s", head.host, head.path);
      printf("Access: %s \n", fullpath);
      fflush(stdout);

      if (
        strncmp(head.path, "/", sizeof("/")) == 0
        || strncmp(head.path, "/index.html", sizeof("/index.html")) == 0
      ) {
        pageFileSize = readPageFile("index.html", buf, sizeof(buf));
        write(connect_socket, buf, sizeof(buf));
      }
      else {
        write(connect_socket, buf404, sizeof(buf404));
      }

      free(head.path);
      free(head.host);
    }

    close(connect_socket);
  }
}

int main(void) {
  // Open socket
  int mainSocket;
  int error = bindSocketToPort(&mainSocket, portNumber);
  if (error < 0) {
    return error;
  }
  
  listening(mainSocket);

  close(mainSocket);
  return 0;
}