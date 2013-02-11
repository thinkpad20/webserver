#include "csapp.h"

void echo (int connfd) 
/* takes a connection file descriptor, prints what is written in file 
by the client and writes the same thing back. */
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, connfd); /*read from the socket into a RIO buffer*/
	while ((n=Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		/* n is the number of bytes read from the buffer. 
		   This will be nonzero until EOF */
		printf("server received %d bytes\n", n);
		Rio_writen(connfd, buf, n); /* write from the buffer back into the FD */
	}
}

int main(int argc, char *argv[]) {
	int listenfd, /*the file descriptor of the server's socket*/
		connfd, /*the file descriptor of the connection*/
		port, /*the port of the server*/
		clientlen; /*the length of the client's address*/

	struct sockaddr_in clientaddr; /*this will store the address of the 
									 client that connects to our server*/
	struct hostent *client_info; /*this will store the host information for the client*/
	char *client_ip; /*this will store the dotted-decimal IP address of the name*/

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	port = atoi(argv[1]);
	listenfd = Open_listenfd(port); /*request a socket fd from the system on port*/
	printf("Server established on port %d, waiting for echo requests...\n", port);

	while(1) {
		clientlen = sizeof(clientaddr);
		/*Accept will return a file descriptor of a shared socket
		  when it makes a connection. It will store the IP in
		  clientaddr*/
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		/*Determine the domain name and IP address of the client*/
		client_info = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		client_ip = inet_ntoa(clientaddr.sin_addr);

		printf("Server connected to %s (%s)\n", client_info->h_name, client_ip);

		echo(connfd);
		Close(connfd);
	}
	exit(0);
}