#include <stdio.h>
#include <winsock2.h>

#include "websocket.h"

int main()
{
    puts("Phase 1: Construct a socket and accept clients\n");
    /*
     * Bind a socket to 0.0.0.0:5000 and listen
     */
    // Initialize Windows sockets library
    // Create the socket
    // Configure socket address struct
    // Bind socket to address
    // Set maximum backlogged (waiting) connections
    // Accept client connections
    // (client address information is not needed, give NULL)
    /*** YOUR CODE HERE ***/
    struct WSAData wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;


    sin.sin_addr.s_addr = ADDR_ANY;
    sin.sin_port = htons(5000);

    bind(serverFd, (struct sockaddr *)&sin, sizeof(sin));
    listen(serverFd, 2);

    struct sockaddr_in sClient;
    int sClientLength;
    SOCKET client_fd = accept(serverFd, (struct sockaddr *) &sClient, &sClientLength);
    printf("Client connected with address=%s and Port=%d \n \n ",inet_ntoa(sClient.sin_addr),ntohs(sClient.sin_port) );


    puts("Phase 2: Receive the upgrade request and respond\n");
    /*
     * Client is connected at this point.
     * A WebSocket client will send an HTTP/1.1 request
     * to upgrade the stream to a WebSocket
     */

    char buffer[4096];
    // clear the buffer before use
    memset(buffer, 0, sizeof(buffer));

    // receive the http request
    int r = recv(client_fd, buffer, sizeof(buffer), 0);

    printf("Client sent upgrade request with %d bytes:\n%s\n", r, buffer);

    // generate the accept key required by the standard
    /*** THIS FUNCTION IS PROVIDED AS-IS, NO MODIFICATION NEEDED ***/
    char *accept_key = make_ws_key(buffer);
    printf("Generated accept key: %s\n", accept_key);

    // send the http upgrade accept response
    /*** REFER TO websocket.h ***/
    send_ws_accept(client_fd, accept_key);
    free(accept_key);

    puts("Phase 3: Communicate over WebSocket protocol\n");
    /*
     * At this stage, WebSocket connection is established
     * Both sides can send and receive WebSocket frames
     * Frame structure is defined in RFC6455
     */

    int running = 1;
    while (running)
    {
        // read user text input (limit length to 125 for simplified frame format)
        char msg_buffer[125];
        printf("Enter your message: ");
        scanf("%s", msg_buffer);

        // clear the buffer before use
        memset(buffer, 0, sizeof(buffer));

        // fill the buffer with websocket text frame containing the message
        /*** REFER TO websocket.h ***/
        size_t frame_len = make_ws_frame(buffer, msg_buffer);
        send(client_fd, buffer, frame_len, 0);

        // clear the buffer before use
        memset(buffer, 0, sizeof(buffer));

        // receive response(s) from the client
        recv(client_fd, buffer, sizeof(buffer), 0);

        // parse the ws frame for text payload or close command
        /*** REFER TO websocket.h ***/
        int opcode = parse_ws_frame(msg_buffer, buffer);

        if (opcode == OPCODE_TEXT)
        {
            printf("Received text: %s\n", msg_buffer);
        }
        else if (opcode == OPCODE_CLOSE)
        {
            puts("Connection closed by the client");
            running = 0;
            break;
        }
        else
        {
            printf("Unsupported frame type (opcode=%x) is received", opcode);
        }
    }

    puts("Phase 4: Cleanup resources\n");
    // Close client socket
    // Close server socket
    // De-initialize Windows sockets
    /*** YOUR CODE HERE ***/
    closesocket(client_fd);
    closesocket(serverFd);
    WSACleanup();
    return 0;
}
