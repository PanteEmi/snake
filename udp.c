#include "udp.h"

struct UDP_client create_socket(const unsigned int port, const char* ip_addr)
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(ip_addr); // Any IP address: "192.168.208.75"
    server_addr.sin_port = htons(port); // Port number: 8080

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("UDP server is listening on port %d...\n", PORT);

    struct UDP_client client = {0};
    client.sockfd = sockfd;
    client.client_addr = client_addr;

    return client;
}


ssize_t receive_from_socket(struct UDP_client client, char *buffer)
{
    socklen_t addr_len = sizeof(client.client_addr);
    ssize_t recv_len = recvfrom(client.sockfd, (char *)buffer, 256, MSG_WAITALL, (struct sockaddr *) &(client.client_addr), &addr_len);
    buffer[recv_len] = '\0';
    printf("Client : %s\n", buffer);
    return recv_len;
}

int socket_wait_for_data(int fd, int timeout_ms) {
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    int select_result = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (select_result == -1) {
        perror("select");
        return -1;
    } else if (select_result == 0) {
        // Timeout occurred, no data available
        return 0;
    } else if (FD_ISSET(fd, &readfds)) {
        // Data is available
        return 1;
    }

    return -1;
}

void close_socket(int sockfd)
{
    close(sockfd);
}
