#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include "../socket_helper.h"

/****************************************
        Author: Tim Wood
        with a little help from
        http://beej.us/guide/bgnet/
****************************************/

int client_main(int sockfd, char* message) {
        int rc;
        /* Send the message, plus the \0 string ending. Use 0 flags. */
        rc = send(sockfd, message, strlen(message)+1, 0);
        if(rc < 0) {
                perror("ERROR on send");
                exit(-1);
        }

        return rc;

}

int main(int argc, char ** argv)
{
        int o;
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        char *message = "Hello World";
        int sockfd;
        int iterations = 1;

        /* Command line args:
                -p port
                -h host name or IP
                -m message to send
                -i number of times to send message (once per second)
        */
        while ((o = getopt (argc, argv, "p:h:m:i:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
                        break;
                case 'm':
                        message = optarg;
                        break;
                case 'i':
                        iterations = atoi(optarg);
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'h' ) {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }
        printf("server_ip: %s   port: %s\n", server_ip, server_port);

        sockfd = socket_connect_helper(server_ip, server_port);

        while(iterations > 0) {
                client_main(sockfd, message);
                printf(".");
                sleep(1);
                iterations--;
        }

        out:
        close(sockfd);

        printf("Done.\n");
        return 0;
}
