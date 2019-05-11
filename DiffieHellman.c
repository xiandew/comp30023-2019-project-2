#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define HOST "172.26.37.44"
#define PORT "7800"
#define USERNAME "xiandew\n"

#define BUFFER_SIZE 2048
#define G 15
#define P 97

int power(int x, unsigned int y, int p);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage %s secret_b\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int B = atoi(argv[1]);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(PORT));
    inet_pton(AF_INET, HOST, &(serv_addr.sin_addr));

    // create socket and connect to the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }


    // send username
    if (write(sockfd, USERNAME, strlen(USERNAME)) < 0) {
        perror("ERROR sending username");
        exit(EXIT_FAILURE);
    }

    // send g^b mod p
    char gb_mod_p[BUFFER_SIZE];
    memset(gb_mod_p, 0, BUFFER_SIZE);
    sprintf(gb_mod_p, "%d\n", power(G, B, P));
    if (write(sockfd, gb_mod_p, strlen(gb_mod_p)) < 0) {
        perror("ERROR sending gb_mod_p");
        exit(EXIT_FAILURE);
    }

    // receive g^a mod p
    char ga_mod_p[BUFFER_SIZE];
    memset(ga_mod_p, 0, BUFFER_SIZE);
	if (read(sockfd, ga_mod_p, BUFFER_SIZE - 1) < 0) {
		perror("ERROR reading ga_mod_p");
		exit(EXIT_FAILURE);
	}

    // send g^ab mod p
    char gab_mod_p[BUFFER_SIZE];
    memset(gab_mod_p, 0, BUFFER_SIZE);
    sprintf(gab_mod_p, "%d\n", power(atoi(ga_mod_p), B, P));
    if (write(sockfd, gab_mod_p, strlen(gab_mod_p)) < 0) {
        perror("ERROR sending gb_mod_p");
        exit(EXIT_FAILURE);
    }

    // receive status report
    char status_report[BUFFER_SIZE];
    memset(status_report, 0, BUFFER_SIZE);
	if (read(sockfd, status_report, BUFFER_SIZE - 1) < 0) {
		perror("ERROR reading ga_mod_p");
		exit(EXIT_FAILURE);
	}
    printf("%s\n", status_report);

    close(sockfd);
    return 0;
}

/**
 * Reference: https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic/
 */
int power(int x, unsigned int y, int p) {
    // Initialize result
     int res = 1;

     // Update x if it is more than or equal to p
     x = x % p;

     while (y > 0) {
         // If y is odd, multiply x with result
         if (y & 1) {
             res = (res*x) % p;
         }

         // y must be even now
         y = y>>1; // y = y/2
         x = (x*x) % p;
     }
     return res;
}
