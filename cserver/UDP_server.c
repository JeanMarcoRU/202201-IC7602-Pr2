#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
#define PORT 53
#define MAXSIZE 2048
int main()
{
    int sock;
    int addr_len, bytes_read;
    char buffer[MAXSIZE];
    struct sockaddr_in server_addr, client_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), 8);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);

    printf("UDPServer Waiting for client on port 53\n");
    fflush(stdout);

    while (1)
    {
        // recibe el mensaje
        bytes_read = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("+----------------+\n\nSe recibió un paquete DNS:\n");
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i+=2)
        {    
            char l[9], h[9];
            sprintf(l, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(buffer[i]));
            sprintf(h, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(buffer[i+1]));
            printf("+----------------+\n|%c%c|\n", buffer[i], buffer[i+1]);
            //printf("+----------------+\n|%s%s|\n", l, h);
        }

        char *end_msj = malloc(MAXSIZE);
        sprintf(end_msj, "Server: %s\n", buffer);

        // envia el mensaje
        if (sendto(sock, end_msj, strlen(end_msj), 0, (struct sockaddr *)&client_addr, addr_len) == -1)
        {
            printf("Error: sendto()");
        }

        fflush(stdout);
    }
    return 0;
}