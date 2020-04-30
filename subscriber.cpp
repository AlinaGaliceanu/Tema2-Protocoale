#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s client_id server_address server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	fd_set read_fds;
    fd_set tmp_fds;
    int fdmax;
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

	if (argc < 4) {
		usage(argv[0]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	FD_SET(0, &read_fds);
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

	while (1) {
		tmp_fds = read_fds;
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (int i = 0; i <= fdmax; ++i) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == sockfd) {
					memset(buffer, 0, BUFLEN);
					int verif =	recv(sockfd, buffer, BUFLEN, 0);
					DIE(verif < 0, "recv");
					if (strncmp(buffer, "exit", 4) == 0) {
						return 0;
					}
					printf("%s\n", buffer);
				} else {
					// se citeste de la tastatura
					memset(buffer, 0, BUFLEN);
					fgets(buffer, BUFLEN - 1, stdin);

					
					char command[12], topic[50];
					int SF = 0;

					sscanf(buffer, "%s %s %d", command, topic, &SF);
					// printf("%s \n %s\n %d\n", command, topic, SF);

					if (strncmp(command, "subscribe", 9) == 0) {
						printf("client abonat la topicul %s\n", topic);	
					}
					if (strncmp(command, "unsubscribe", 11) == 0) {
						printf("client dezabonat de la topicul %s\n", topic);
					}
					if (strncmp(buffer, "exit", 4) == 0) {
						return 0;
					} 
						// se trimite mesaj la server
					n = send(sockfd, buffer, strlen(buffer), 0);
					DIE(n < 0, "send");
				}
			}
		}
	}

	close(sockfd);

	return 0;
}