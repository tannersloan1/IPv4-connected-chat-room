#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 4096
#define MESSAGE_SIZE 256

int main(int argc, char* argv[]) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char message_buffer[BUFFER_SIZE];
    char line[MESSAGE_SIZE];
    char recv_buffer[BUFFER_SIZE];
    int port;
    int local_exit = 0, remote_exit = 0;
    int my_turn = 0;  // Server waits for client to send first
    char pres_addr[INET_ADDRSTRLEN];

    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please use a port between 1 and 65535.\n");
        return 1;
    }

    // Initialize Winsock
    printf("Initializing Winsock.\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Bind done\n");

    // Listen
    listen(server_socket, 1);
    printf("Waiting for incoming connection on port %d.\n", port);

    // Accept connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    char* remote_ip = inet_ntop(AF_INET, &client_addr.sin_addr, pres_addr, sizeof(pres_addr));
    int remote_port = ntohs(client_addr.sin_port);
    printf("Connection accepted from %s:%d\n", remote_ip, remote_port);
    printf("Chat started! Waiting for User2 to send first message.\n");

    // Main communication loop
    while (!local_exit || !remote_exit) {

        if (!my_turn) {
            // Receive messages
            int recv_size = recv(client_socket, recv_buffer, BUFFER_SIZE - 1, 0);

            if (recv_size == SOCKET_ERROR || recv_size == 0) {
                printf("Connection closed by User2 via error.\n");
                break;
            }

            recv_buffer[recv_size] = '\0';

            // Display received messages
            if (strlen(recv_buffer) > 0) {
                // Check if message contains exit tag
                if (strstr(recv_buffer, "[EXIT]") != NULL) {

                    remote_exit = 1;

                    // Remove the exit tag
                    char* exit_tag = strstr(recv_buffer, "[EXIT]\n");
                    if (exit_tag != NULL) {
                        *exit_tag = '\0';
                    }

                    printf("%s", recv_buffer);
                    printf("\nUser2: I want to exit.\n");
                    if (local_exit) {  // Both want to exit and user1(server) was last to exit
                        printf("\nConnection ended by User2.\n");
                        break;
                    }
                    my_turn = 1;

                    // Prompt to start composing
                    printf("\nPress '#' and Enter to start composing your message.\n");
                    fgets(line, MESSAGE_SIZE, stdin);

                    // Keep prompting for # until user enters it
                    while (strcmp(line, "#\n") != 0) {
                        printf("Press '#' and Enter to start composing your message.\n");
                        fgets(line, MESSAGE_SIZE, stdin);
                    }
                    continue;
                }
                printf("%s", recv_buffer);
            }

            my_turn = 1;

            // Prompt to start composing
            printf("\nPress '#' and Enter to start composing your message.\n");
            fgets(line, MESSAGE_SIZE, stdin);

            // Keep prompting for # until user enters it
            while (strcmp(line, "#\n") != 0) {
                printf("Press '#' and Enter to start composing your message.\n");
                fgets(line, MESSAGE_SIZE, stdin);
            }
        }

        if (my_turn) {
            // Compose messages
            message_buffer[0] = '\0';
            strcat_s(message_buffer, sizeof(message_buffer), "\n");

            printf("\n");
            while (1) {
                printf("Enter a message: ");
                fgets(line, MESSAGE_SIZE, stdin);

                // Check for #
                if (strcmp(line, "#\n") == 0) {
                    break;
                }

                // Check for exit
                if (strcmp(line, "Exit\n") == 0) {
                    strcat_s(message_buffer, sizeof(message_buffer), "[EXIT]\n");
                    local_exit = 1;
                    break;
                }

                // Add to buffer with "User1: " prefix
                strcat_s(message_buffer, sizeof(message_buffer), "User1: ");
                strcat_s(message_buffer, sizeof(message_buffer), line);
            }

            // Send the current content of the message buffer after # or Exit breaks getting messages loop
            if (strlen(message_buffer) > 0) {
                if (send(client_socket, message_buffer, strlen(message_buffer), 0) == SOCKET_ERROR) {
                    printf("Send failed with error code: %d\n", WSAGetLastError());
                    break;
                }
            }

            my_turn = 0;

            if (local_exit && remote_exit) {
                break;
            }
        }
    }

    printf("\nChat ended. Closing connection.\n");
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}