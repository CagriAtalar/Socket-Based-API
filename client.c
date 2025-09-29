// client.c
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        printf("Usage: ./client push <word> | ./client pop\n");
        return 1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    char message[1024];
    if (strcasecmp(argv[1], "push") == 0 && argc >= 3) {
        int len1 = strlen("push");
        int len2 = strlen(argv[2]);
        snprintf(message, sizeof(message), "%d#%s%d#%s", len1, "push", len2, argv[2]);
    } else if (strcasecmp(argv[1], "pop") == 0) {
        int len1 = strlen("pop");
        snprintf(message, sizeof(message), "%d#%s", len1, "pop");
    } else {
        printf("Invalid command\n");
        return 1;
    }

    send(sock, message, strlen(message), 0);
    printf("Sent: %s\n", message);

    int valread = read(sock, buffer, sizeof(buffer)-1);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
