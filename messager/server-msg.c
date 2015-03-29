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
****************************************/


/* Main server logic */
void server_main(int sockfd) {

        while(1) {
                struct sockaddr_storage client_addr;
                socklen_t addr_size;
                int clientfd;
                int bytes_read;
                char message[256];

                addr_size = sizeof client_addr;
                clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
                print_client_ip(client_addr);

                /* Keep receiving strings until client disconnects. */
                while(1) {
                        bytes_read = read(clientfd, message, sizeof message);
                        if(bytes_read <= 0) {
                                printf("Client disconnected.\n");
                                close(clientfd);
                                break;
                        }
                        message[bytes_read] = 0; /* telnet fix: ensure no extra garbage is included. */
                        printf("Message (%d): [%s]\n", bytes_read, message);
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
        while ((o = getopt (argc, argv, "p:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
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
