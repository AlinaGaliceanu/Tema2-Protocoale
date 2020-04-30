#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "helpers.h"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr, cli_udp;
	int n, i, ret;
	socklen_t clilen;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}
	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(3105);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");
    int MAX_CLIENTS = 2;
	ret = listen(sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

    //Deschidere socket udp
    int sock_udp;
    sock_udp = socket(AF_INET, SOCK_DGRAM, 0);

    ret = bind(sock_udp, (struct sockaddr*) &serv_addr,sizeof(struct sockaddr));
    DIE(ret, "bind");

	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(sockfd, &read_fds);
    FD_SET(sock_udp, &read_fds);

	fdmax = (sockfd > sock_udp) ? sockfd : sock_udp;
 
	while (1) {
		tmp_fds = read_fds; 
		
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		int sock_fds[10], number_of_clients = 0;
        struct sockaddr_in in2;
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				if (i == sockfd) {
					// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					DIE(newsockfd < 0, "accept");
	
					// se adauga noul socket intors de accept() la multimea descriptorilor de citire
					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax) { 
						fdmax = newsockfd;
					}
		
					printf("New client %d connected from %s:%d\n",
							 newsockfd, inet_ntoa(cli_addr.sin_addr), htons(cli_addr.sin_port));
			
				} else {
					
                    if (i == sock_udp) {
					// s-au primit date pe unul din socketii de client,
					// asa ca serverul trebuie sa le receptioneze
					memset(buffer, 0, BUFLEN);
					struct sockaddr_in in;
					socklen_t socklen;
					n = recvfrom(sock_udp, buffer, BUFLEN, 0, (struct sockaddr *) &in, &socklen);
					DIE(n < 0, "recv");
					if (n == 0) {
						// conexiunea s-a inchis
						printf("Client %d disconnected\n", i);
						close(i);
						
						// se scoate din multimea de citire socketul inchis 
						FD_CLR(i, &read_fds);
					} 
					if (i == sock_udp) {
                    
						char aux[50]; //numele topicului
						int aux1; // tipul de date (0 1 2 sau 3)
						memcpy(aux, buffer, 50);
						aux1 = buffer[50];
						int sign = buffer[51];
						if (aux1 == 0) {
							uint32_t aux2; // continut topic (valoare efectiva)
							memcpy(&aux2, buffer + 52, sizeof(uint32_t));	
							int value = ntohl(aux2);
							if (sign == 1) {
								value = value * (-1);
							}		
							printf ("S-a primit de la clientul de pe socketul %d mesajul: %s %d %d\n", i, aux, aux1, value) ;
						}

						if (aux1 == 1) {
							uint16_t aux3;
							memcpy(&aux3, buffer + 51, sizeof(uint16_t));		
						
							printf ("S-a primit de la clientul de pe socketul %d mesajul: %s %d %.2f\n", i, aux, aux1, htons(aux3)/100.00) ;
						}
				
						if (aux1 == 2) {
							uint32_t aux3;
							memcpy(&aux3, buffer + 52, sizeof(uint32_t));				
							uint8_t aux4;
							memcpy(&aux4, buffer + 52 + sizeof(uint32_t), sizeof(uint8_t));		
							int pow = 1;
							for (int i = 0; i < aux4; ++i) {
								pow = pow * 10;
							}
							float value  = ntohl(aux3) * 1.00 / pow;
							if (sign == 1) {
								value = value * (-1);
							}
							printf ("S-a primit de la clientul de pe socketul %d mesajul: %s %d %.8g %d\n", i, aux, aux1, value, aux4) ;
						}

						if (aux1 == 3) {
							char aux3[1501];
							memcpy(aux3, buffer + 51, 1500);
							printf ("S-a primit de la clientul de pe socketul %d mesajul: %s %d %s\n", i, aux, aux1, aux3) ;

						}
					
						} 
						} else {
							memset(buffer, 0, BUFLEN);

							fgets(buffer, BUFLEN - 1, stdin);
							printf("%s\n", buffer);
							if (strncmp(buffer, "exit", 4) == 0) {
								for (int sockfd: sock_fds) {
									send(sockfd, "exit", 4, 0);
									close(sockfd);
									FD_CLR(sockfd, &read_fds);
								}
								return 0;
							} 
						}
				}
        
			}
		}
	}

	close(sockfd);
    close(sock_udp);
	return 0;
}
