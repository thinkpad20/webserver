#include "csapp.h"

void doit(int file_des);
void read_requesthdrs(rio_t *rio_ptr);
int parse_uri(char *uri, char *filename, char *cgi_args);
void serve_static(int file_des, char *filename, int filesize);
void serve_dynamic(int file_des, char *filename, char *cgi_args);
void get_filetype(char *filename, char *filetype);
void send_error(int file_des, char *cause, char *errnum, char *shortmsg, char *longmsg);

#define DEFAULT_PAGE "index.html"
#define SERVER_NAME L33T

int main(int argc, char *argv[]) 
{
	int listen_fd, /*descriptor for server*/
		conn_fd,  /*descriptor for connection*/
		port,
		clientlen;
	struct sockaddr_in client_info; /*stores the client's socket info*/

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	port = atoi(argv[1]);

	listen_fd = Open_listenfd(port);

	printf("L33T s3rv3r up and running, waiting for connections...\n");

	/*set up listening loop*/
	while (1) {
		clientlen = sizeof(client_info);
		conn_fd = Accept(listen_fd, (SA *)&client_info, &clientlen);
		doit(conn_fd);
		Close(conn_fd);
	}

	exit(0);
}

void doit(int file_des)
{
	int is_static;
	struct stat file_stat;
	char buf[MAXLINE], /*multi-purpose buffer*/
		 method[MAXLINE],
		 uri[MAXLINE], version[MAXLINE],
		 filename[MAXLINE], cgi_args[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, file_des); /*initialize our rio struct to read from the socket*/
	Rio_readlineb(&rio, buf, MAXLINE); /*read up to MAXLINE from the socket*/
	sscanf(buf, "%s %s %s", method, uri, version); /*get the command from the user
													for example, "GET / HTTP/1.1"*/

	if (strcasecmp(method, "GET") != 0) { /*if the method is not GET*/
		send_error(file_des, method, "501", "Not Implemented", 
			"L33T s3rv3r does not implement this method :(");
		return;
	}
	read_requesthdrs(&rio); /*throw away the request headers*/

	is_static = parse_uri(uri, filename, cgi_args);
	/* try to load the statistics of that file */
	fprintf(stderr, "Attempting to load file %s:\n", filename);
	if (stat(filename, &file_stat) < 0) { /*then the file was not found*/
		send_error(file_des, filename, "404", "Not found", 
			"L33T s3rv3r couldn't find the file (T__T)");
		return;
	}

	if (is_static) {
		/*check if we have permission to read the file*/
		if (!(S_ISREG(file_stat.st_mode)) || !(S_IRUSR & file_stat.st_mode)) {
			send_error(file_des, filename, "403", "Forbidden", 
				"L33T s3rv3r could not read the file :(");
			return;
		}
		serve_static(file_des, filename, file_stat.st_size);
	}

	else { /*serve dynamic content*/
		if (!(S_ISREG(file_stat.st_mode)) || !(S_IXUSR & file_stat.st_mode)) {
			send_error(file_des, filename, "403", "Forbidden", 
				"L33T s3rv3r could not read the file :(");
			return;
		}
		serve_dynamic(file_des, filename, cgi_args);
	}

}

void send_error(int file_des, char *cause, char *errnum, char *shortmsg, char *longmsg) 
{
	char buf[MAXLINE], body[MAXBUF];

	/*Build the response message*/
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The L33T server</em>\r\n", body);

	/*Write the HTTP response and send it to the socket*/
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(file_des, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(file_des, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(file_des, buf, strlen(buf));
	Rio_writen(file_des, body, strlen(body));
}

void read_requesthdrs(rio_t *riop) {
	char buf[MAXLINE];
	Rio_readlineb(riop, buf, MAXLINE);
	while (strcmp(buf, "\r\n") != 0) { /*keep going until we see just a \r\n (empty line)*/
		Rio_readlineb(riop, buf, MAXLINE);
		printf("%s", buf); /*this isn't really necessary to do but it's good for logging*/
	}
}

int parse_uri(char *uri, char *filename, char *cgiargs) 
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {  /* Static content */
		strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri)-1] == '/')
		    strcat(filename, DEFAULT_PAGE);
		return 1;
    }
    else {  /* Dynamic content */
		ptr = index(uri, '?');
		if (ptr) {
		    strcpy(cgiargs, ptr+1);
		    *ptr = '\0';
		} else {
		    strcpy(cgiargs, "");
		}
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
    }
}

void get_filetype(char *filename, char *filetype)
{
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/png");
	else
		strcpy(filetype, "text/plain");
}

void serve_static(int file_des, char *filename, int filesize) 
{
	int source_fd;
	char *source_ptr, filetype[MAXLINE], buf[MAXBUF];

	get_filetype(filename, filetype); /*load the file type into filetype*/
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: L33T Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(file_des, buf, strlen(buf));

	source_fd = Open(filename, O_RDONLY, 0);
	/*use mmap to load the hard drive data into memory somewhere*/
	source_ptr = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, source_fd, 0);
	/*close the file; we don't need it any more since it's in memory*/
	Close(source_fd);
	/*write the data into the socket*/
	Rio_writen(file_des, source_ptr, filesize);
	/*unmap to free the memory that map allocated*/
	Munmap(source_ptr, filesize);
}

void serve_dynamic(int file_des, char *filename, char *cgi_args)
{
	char buf[MAXLINE], *emptylist[] = {NULL};

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	Rio_writen(file_des, buf, strlen(buf));
	sprintf(buf, "Server: L33T Server\r\n");
	Rio_writen(file_des, buf, strlen(buf));

	/*fork off a child process to run the CGI program*/
	if (Fork() == 0) {
		setenv("QUERY_STRING", cgi_args, 1);
		Dup2(file_des, STDOUT_FILENO); /*redirect stdout to the socket*/
		/*run the CGI program with an empty argument list*/
		Execve(filename, emptylist, environ);
	}
	Wait(NULL); /*wait for all child processes to complete and reap them*/
}

