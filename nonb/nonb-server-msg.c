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
#include <fcntl.h> /* Needed for non-blocking IO */
#include "../socket_helper.h"

/****************************************
        Author: Tim Wood
        with a little help from
        http://beej.us/guide/bgnet/

        Nonblocking server that can receive messages from clients
        and prints them out to screen. Each thread uses a non-blocking
        socket that it polls to see when new messages arrive.

****************************************/

int SINGLE_THREAD = 0;

struct client_info {
        int fd;
        struct sockaddr_storage addr;
};

void* server_thread(struct client_info* client) {
        int bytes_read;
        char message[256];


        print_client_ip(client->addr);

        /* Set socket to be NON-Blocking */
        fcntl(client->fd, F_SETFL, O_NONBLOCK);

        while(1) {
                int offset = 0;
                bytes_read = read(client->fd, message, sizeof message);
                if(bytes_read > 0) {
                        /* The socket has data ready */
                        message[bytes_read] = 0; /* telnet fix: ensure no extra garbage is included. */
                        /* One read call may bring back several strings. Iterate through them */
                        while(offset < bytes_read) {
                                printf("Thread %d Message (%d): [%s]\n", pthread_self(), bytes_read, message + offset);
                                offset += strlen(message + offset) + 1; /* skip string terminator */
                        }
                }
                else if(bytes_read == 0) {
                        /* Client closed socket */
                        break;
                }
                else {
                        /* No data is waiting on socket... */
                        printf("Thread %d has no new data!\n", pthread_self());
                        usleep(100000); /* Sleep for 100 millisec */
                }
        }

        close(client->fd);
        free(client);
        pthread_detach(pthread_self());

}

/* Main server logic */
void server_main(int sockfd) {

        while(1) {
                socklen_t addr_size;
                pthread_t thread;
                struct client_info* client = (struct client_info*) malloc(sizeof(struct client_info));
                addr_size = sizeof client->addr;

                client->fd = accept(sockfd, (struct sockaddr *)&client->addr, &addr_size);
                if(SINGLE_THREAD == 1) {
                        server_thread(client);
                }
                else {
                        pthread_create(&thread, NULL, (void*) server_thread, (void*) client);
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
        */
        while ((o = getopt (argc, argv, "p:s")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 's':
                        SINGLE_THREAD = 1;
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
