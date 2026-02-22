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
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char message_buffer[BUFFER_SIZE];
    char line[MESSAGE_SIZE];
    char recv_buffer[BUFFER_SIZE];
    char* server_ip;
    int port;
    int local_exit = 0, remote_exit = 0;
    int my_turn = 1;  // Client sends first

    // Check command line arguments
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    server_ip = argv[1];
    port = atoi(argv[2]);

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
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid Address: %s\n", server_ip);
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Connect to server
    printf("Connecting to %s:%d.\n", server_ip, port);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connect failed. Error Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected successfully!\n");
    printf("Chat started! You can start composing your first message.\n");

    // Main communication loop
    while (!local_exit || !remote_exit) {
        if (my_turn) {
            // Write messages
            message_buffer[0] = '\0';
            strcat_s(message_buffer, sizeof(message_buffer), "\n");

            printf("\n");

            // Continuously let the user enter lines of messages until # or Exit is sent
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

                // Add message to buffer with "User2: " prefix
                strcat_s(message_buffer, sizeof(message_buffer), "User2: ");
                strcat_s(message_buffer, sizeof(message_buffer), line);
            }

            // Send the current messages in the buffer after either # or Exit breaks the previous loop
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

        if (!my_turn) {
            // Receive messages from server
            int recv_size = recv(client_socket, recv_buffer, BUFFER_SIZE - 1, 0);

            if (recv_size == SOCKET_ERROR || recv_size == 0) {
                printf("Connection closed by User1 via an error.\n");
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

                    printf("%s", recv_buffer);  // Display any messages before the Exit was sent
                    printf("\nUser1: I want to exit.\n");
                    if (local_exit) {  // Both want to exit and user1(server) was last to exit
                        printf("\nConnection ended by User1.\n");
                        break;
                    }
                    my_turn = 1;
                    // Prompt to start composing
                    printf("\nPress '#' and Enter to start composing your message.\n");
                    fgets(line, MESSAGE_SIZE, stdin);

                    // Wait for # to start
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

            // Keep waiting for #
            while (strcmp(line, "#\n") != 0) {
                printf("Press '#' and Enter to start composing your message.\n");
                fgets(line, MESSAGE_SIZE, stdin);
            }
        }
    }

    printf("\nChat ended. Closing connection.\n");
    closesocket(client_socket);
    WSACleanup();

    return 0;
}