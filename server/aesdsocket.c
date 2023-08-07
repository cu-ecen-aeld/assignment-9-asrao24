// server/aesdsocket.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <syslog.h>
#include <signal.h>
#include <arpa/inet.h>

#define PORT 9000
#define DATA_FILE_PATH "/var/tmp/aesdsocketdata"
#define NET_BUF_SIZE 1024

volatile sig_atomic_t g_running = true;

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        g_running = false;
        syslog(LOG_USER | LOG_INFO, "Caught signal, exiting");
    }
}

int main() {
    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    openlog("aesdsocket", LOG_PID, LOG_USER);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up server address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        return -1;
    }

    while (g_running) {
        // Accept a new connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket == -1) {
            perror("Accept failed");
            return -1;
        }

        char client_addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), client_addr_str, INET_ADDRSTRLEN);
        syslog(LOG_USER | LOG_INFO, "Accepted connection from %s", client_addr_str);

        FILE *fp = fopen(DATA_FILE_PATH, "a");
        if (fp == NULL) {
            perror("Error opening data file");
            return -1;
        }

        char recv_buf[NET_BUF_SIZE];
        int bytes_read;

        while (true) {
            bytes_read = recv(new_socket, recv_buf, NET_BUF_SIZE - 1, 0);
            if (bytes_read <= 0) {
                // Either an error or client closed connection
                break;
            }

            recv_buf[bytes_read] = '\0';

            // Look for newline in the received buffer
            char *newline_pos = strchr(recv_buf, '\n');
            if (newline_pos != NULL) {
                *newline_pos = '\0'; // Null-terminate at the newline
                fprintf(fp, "%s\n", recv_buf);
                fflush(fp);

		printf("received string:%s\n", recv_buf);

                // Return data from file to the client
                fseek(fp, 0, SEEK_SET);
                while (fgets(recv_buf, NET_BUF_SIZE, fp) != NULL) {
                    send(new_socket, recv_buf, strlen(recv_buf), 0);
                }
            } else {
                // If no newline, keep appending to the file
                fprintf(fp, "%s", recv_buf);
                fflush(fp);
            }
        }

        fclose(fp);
        close(new_socket);
        syslog(LOG_USER | LOG_INFO, "Closed connection from %s", client_addr_str);
    }

    // Clean up before exiting
    close(server_fd);
    remove(DATA_FILE_PATH);
    closelog();

    return 0;
}

