#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <arpa/inet.h>
#include "../socket_helper.h"

/****************************************
        Author: Tim Wood
        with a little help from
        http://beej.us/guide/bgnet/

        Select based server that can receive messages from multiple clients
        and prints them out to screen. Only uses a single thread, but relies
        on select() to determine which sockets have data ready.

        This waits for all NUM_CLIENTS to connect before reading any messages,
        in reality you probably want to have select monitor the listening
        socket as well and handle new accepts.

        Use -c to set the number of clients to expect.

****************************************/

int NUM_CLIENTS = 2;

struct client_info {
        int fd;
        struct sockaddr_storage addr;
};

/* Call read on a socket and print any strings that have been sent. */
int read_msg(int clientfd) {
        int bytes_read;
        char message[256];
        int offset = 0;

        bytes_read = read(clientfd, message, sizeof message);
        if(bytes_read > 0) {
                /* The socket has data ready */
                message[bytes_read] = 0; /* telnet fix: ensure no extra garbage is included. */
                /* One read call may bring back several strings. Iterate through them */
                while(offset < bytes_read) {
                        printf("Client %d Message (%d): [%s]\n", clientfd, bytes_read, message + offset);
                        offset += strlen(message + offset) + 1; /* skip string terminator */
                }
        }
        return bytes_read;
}

/* Main server logic
        - Call accept NUM_CLIENTS time
        - then use select to monitor all sockets and print any incoming msgs
 */
void server_main(int listenfd) {

        socklen_t addr_size;
        int fdmax;

        fd_set master_fds; // full list of FDs
        fd_set active_fds; // list we will pass to select()
        /* Now use select to see who has sent data */
        FD_ZERO(&master_fds);    // clear the master and temp sets
        FD_ZERO(&active_fds);

        /* Let NUM_CLIENTS connect before handling any messages */
        int c;
        struct client_info* client[NUM_CLIENTS];
        for(c=0; c < NUM_CLIENTS; c++) {
                /* Handle a new connection */
                client[c] = (struct client_info*) malloc(sizeof(struct client_info));
                addr_size = sizeof client[c]->addr;
                client[c]->fd = accept(listenfd, (struct sockaddr *)&client[c]->addr, &addr_size);
                FD_SET(client[c]->fd, &master_fds); // add listener to our set
                printf("Accepted Client %d with socket %d\n", c, client[c]->fd);
                if(client[c]->fd > fdmax) {
                        // keep track of the biggest file descriptor
                        fdmax = client[c]->fd; // so far, it's this one
                }

        }

        while(1) {
                /* Now use select to see if any of the clients have sent data */
                active_fds = master_fds;
                if(select(fdmax+1, &active_fds, NULL, NULL, NULL) == -1) {
                        perror("ERROR with select!");
                        exit(1);
                }
                for(c=0; c < NUM_CLIENTS; c++) {
                        if(FD_ISSET(client[c]->fd, &active_fds)) {
                                int s = read_msg(client[c]->fd);
                                if (s <= 0) {
                                        printf("Client %d disconnected\n", client[c]->fd);
                                        FD_CLR(client[c]->fd, &master_fds);
                                }
                        }
                }
        }
}


int main(int argc, char ** argv)
{
        char* server_port = "1234";
        int sockfd;
        int o;

        /* Command line args:
                -p port
                -c number of clients to accept before reading data
        */
        while ((o = getopt (argc, argv, "p:sc:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'c':
                        NUM_CLIENTS = atoi(optarg);
                        break;
                case '?':
                        if(optopt == 'p') {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("listening on port: %s\n", server_port);

        sockfd = socket_server_helper(server_port);

	/* Loop forever accepting new connections. */
        server_main(sockfd);

        out:
        close(sockfd);

        printf("Done.\n");
        return 0;
}
