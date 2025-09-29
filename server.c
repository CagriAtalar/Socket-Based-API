#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "opcode.h"

#define PORT 8080

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int socket;
} thread_arg_t;

typedef struct {
    OPCODE op;
    char argument[512];
    int has_arg;
} DecodedMessage;

void send_trap(int sock, TRAP_TABLE trap) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "TRAP:%d", trap);
    send(sock, buffer, strlen(buffer), 0);
}

OPCODE str_to_opcode(const char* str) {
    if (strcasecmp(str, "push") == 0) return OPCODE_PUSH;
    if (strcasecmp(str, "pop") == 0) return OPCODE_POP;
    return OPCODE_UNKNOWN;
}

// -------------------- DECODE --------------------
TRAP_TABLE decode_message(const char* buffer, DecodedMessage* out) {
    printf("Raw buffer: '%s'\n", buffer);

    char* ptr = (char*)buffer;

    // 1) opcode length oku
    int len1 = atoi(ptr);
    char* p = strchr(ptr, '#');
    if (!p) {
        return TRAP_DECODE_ERROR;
    }

    p++; // '#' sonrası opcode
    char opcode_str[64] = {0};
    strncpy(opcode_str, p, len1);
    opcode_str[len1] = '\0';

    out->op = str_to_opcode(opcode_str);
    printf("Decoded opcode: '%s'\n", opcode_str);

    // 2) arg varsa oku
    out->has_arg = 0;
    out->argument[0] = '\0';

    char* after_opcode = p + len1;
    if (*after_opcode != '\0') {
        int len2 = atoi(after_opcode);
        char* p2 = strchr(after_opcode, '#');
        if (!p2) return TRAP_DECODE_ERROR;

        strncpy(out->argument, p2+1, len2);
        out->argument[len2] = '\0';
        out->has_arg = 1;
        printf("Decoded argument: '%s'\n", out->argument);
    }

    return TRAP_OK;
}

// -------------------- PUSH --------------------
TRAP_TABLE handle_push(const char* data) {
    pthread_mutex_lock(&file_mutex);
    FILE* f = fopen("DATABASE.txt", "a");
    if (!f) {
        pthread_mutex_unlock(&file_mutex);
        return TRAP_FILE_ERROR;
    }
    fprintf(f, "%s\n", data);
    fclose(f);
    pthread_mutex_unlock(&file_mutex);
    return TRAP_OK;
}

// -------------------- THREAD FUNC --------------------
void* progress(void* arg) {
    thread_arg_t* t_arg = (thread_arg_t*)arg;
    int sock = t_arg->socket;
    free(t_arg);

    char buffer[1024] = {0};
    int valread = read(sock, buffer, sizeof(buffer)-1);
    if (valread <= 0) {
        close(sock);
        pthread_exit(NULL);
    }

    DecodedMessage msg;
    TRAP_TABLE trap = decode_message(buffer, &msg);
    if (trap != TRAP_OK) {
        send_trap(sock, trap);
        close(sock);
        pthread_exit(NULL);
    }

    switch (msg.op) {
        case OPCODE_PUSH:
            if (msg.has_arg)
                trap = handle_push(msg.argument);
            else
                trap = TRAP_DECODE_ERROR;
            break;

        case OPCODE_POP:
            pthread_mutex_lock(&file_mutex);
            {
                FILE *f = fopen("DATABASE.txt", "r");
                if (!f) {
                    trap = TRAP_FILE_ERROR;
                } else {
                    char lines[1024][256];
                    int count=0;
                    while (fgets(lines[count], sizeof(lines[count]), f)) {
                        count++;
                    }
                    fclose(f);
                    if (count>0) {
                        lines[count-1][strcspn(lines[count-1], "\n")] = 0;
                        send(sock, lines[count-1], strlen(lines[count-1]), 0);

                        f = fopen("DATABASE.txt", "w");
                        for (int i=0; i<count-1; i++) {
                            fputs(lines[i], f);
                        }
                        fclose(f);
                        trap = TRAP_OK;
                    } else {
                        trap = TRAP_FILE_ERROR;
                    }
                }
            }
            pthread_mutex_unlock(&file_mutex);
            break;

        default:
            trap = TRAP_UNKNOWN_OPCODE;
    }

    send_trap(sock, trap);

    close(sock);
    pthread_exit(NULL);
}

// -------------------- MAIN --------------------
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }
        thread_arg_t* t_arg = malloc(sizeof(thread_arg_t));
        t_arg->socket = new_socket;
        pthread_t tid;
        pthread_create(&tid, NULL, progress, t_arg);
        pthread_detach(tid);
    }

    return 0;
}
