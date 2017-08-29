#include "httpd.h"

const char *status_line = "HTTP/1.0 200 OK\r\n";
const char *type_line="Content-Type: text/html;charset=ISO-8859-1\r\n";
const char *blank_line  = "\r\n";

int startup(const char *ip, int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = inet_addr(ip);

	if( bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0 ){
		perror("bind");
		exit(3);
	}

	if(listen(sock, 10)){
		perror("listen");
		exit(4);
	}

	return sock;
}

//\r\n \n \r  -> \n
static int get_line(int sock, char buf[], int size)
{
	int i = 0;
	char c = '\0';
	while( i < size - 1 && c != '\n' ){
		ssize_t s = recv(sock, &c, 1, 0);
		if(s > 0){
			if(c == '\r'){
				recv(sock, &c, 1, MSG_PEEK);
				if(c == '\n'){
					recv(sock, &c, 1, 0);
				}else{
					c = '\n';
				}
			}//\r -> \n || \r\n -> \n
			buf[i++] = c;
		}else{
			return -1;
		}
	}
	buf[i] = 0;
	return i;
}

int echo_www(int sock,const char *path, int size)
{
	int fd = open(path, O_RDONLY);
	if(fd < 0){
		perror("open");
		return 404;
	}
	send(sock, status_line, strlen(status_line), 0);
	send(sock, blank_line, strlen(blank_line), 0);
	sendfile(sock, fd, NULL, size);
	close(fd);
}

void handler_header(int sock)
{
	char buf[SIZE];
	int  ret = -1;
	do{
		ret = get_line(sock, buf, sizeof(buf));
	}while(ret > 0 && strcmp(buf, "\n") != 0);
}

void show_404(int sock)
{
	char buf[SIZE];
	strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
	send(sock, buf, strlen(buf), 0);
	send(sock, type_line, strlen(type_line), 0);
	strcpy(buf, "\r\n");
	send(sock, buf, strlen(buf), 0);
	strcpy(buf, "<html><h2>Not Found! :(</h2></html>\r\n");
	send(sock, buf, strlen(buf), 0);
}

void error_response(int sock, int err_code)
{
	switch(err_code){
		case 200:
			break;
		case 301:
			break;
		case 401:
			break;
		case 404:
			show_404(sock);
			break;
		case 501:
			break;
		case 503:
			break;
		default:
			break;
	}
}

int exe_cgi(int sock, char *path, char *method, char *query_string)
{
	char buf[SIZE];
	int ret = -1;
	int content_len = -1;
	int input[2];
	int output[2];
	char content_len_env[SIZE];
	char method_env[SIZE];
	char query_string_env[SIZE];

	if(strcasecmp(method, "GET") == 0){
		handler_header(sock);
	}else{//POST
		do{
			ret = get_line(sock, buf, sizeof(buf));
			if(ret > 0 &&\
					strncmp(buf, "Content-Length: ", 16) == 0){
				content_len = atoi(&buf[16]);
			}
		}while(ret > 0 && strcmp(buf, "\n") != 0);
		if(content_len < 0){
			return 404;
		}
	}
	if(pipe(input) < 0){
		perror("pipe");
		return 404;
	}
	if(pipe(output) < 0){
		perror("pipe");
		close(input[0]);
		close(input[1]);
		return 404;
	}
	//path, running, query_string, sock->content_len
	
	send(sock, status_line, strlen(status_line), 0);
	send(sock, type_line, strlen(type_line), 0);
	send(sock, blank_line, strlen(blank_line), 0);

	pid_t id = fork();
	if( id < 0 ){
		perror("fork");
	}else if(id == 0){//child
		close(input[1]);
		close(output[0]);
		dup2(input[0], 0);
		dup2(output[1], 1);
		//name=value
		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);

		if(strcasecmp(method, "GET") == 0){
			sprintf(query_string_env,\
					"QUERY_STRING=%s", query_string);
			putenv(query_string_env);
		}else{
			sprintf(content_len_env, "CONTENT_LENGTH=%d", content_len);
			putenv(content_len_env);
		}

		execl(path, path, NULL);
		exit(1);
	}else{//father
		close(input[0]);
		close(output[1]);

		int i = 0;
		char c;
		if(strcasecmp(method, "POST") == 0){
			for( ; i < content_len; i++ ){
				recv(sock, &c, 1, 0);
				write(input[1], &c, 1);
			}
		}

		while(read(output[0], &c, 1) > 0){
			send(sock, &c, 1, 0);
		}

		waitpid(id, NULL, 0);
		close(input[1]);
		close(output[0]);
	}
}

void *handler_request(void *arg)
{
	int sock = (int)arg;
	char buf[SIZE];
	char method[SIZE/10];
	char url[SIZE];
	char path[SIZE];
	int err_code = 200;
	int cgi = 0;
	char *query_string = NULL;
	int i, j;
#ifdef _STDOUT_
	ssize_t s = read(sock, buf, sizeof(buf)-1);
	buf[s] = 0;
	printf("client: %s", buf);
#endif
	if(get_line(sock, buf, sizeof(buf)) <= 0){
		err_code = 404;
		goto end;
	}
	printf("%s", buf);
	i = 0;
	j = 0;
	while(i < sizeof(method)-1 && j < sizeof(buf) &&\
			!isspace(buf[j])){
		method[i] = buf[j];
		i++, j++;
	}
	method[i] = 0;
	// GET      /a/b/c.html HTTP/1.0
	while(j < sizeof(buf) && isspace(buf[j])){
		j++;
	}

	i=0;
	while(i < sizeof(url)-1 && j < sizeof(buf) &&\
			!isspace(buf[j])){
		url[i] = buf[j];
		i++, j++;
	}
	url[i] = 0;
	if(strcasecmp(method, "GET") &&\
			strcasecmp(method, "POST")){
		err_code = 404;
		goto end;
	}

	if(strcasecmp(method, "POST") == 0){
		cgi = 1;
	}else{
		query_string = url;
		while(*query_string){
			if( *query_string == '?' ){
				*query_string = '\0';
				query_string++;
				cgi = 1;
				break;
			}
			query_string++;
		}
	}
	//url ->  wwwroot/ wwwroot/a/b/c
	sprintf(path, "wwwroot%s", url);
	if(path[strlen(path)-1] == '/'){
		strcat(path, "index.html");
	}

	struct stat st;
	if(stat(path, &st) < 0){
		err_code = 404;
		goto end;
	}else{
		if(S_ISDIR(st.st_mode)){
			strcat(path, "/index.html");
		}else if((st.st_mode & S_IXUSR) || \
				 (st.st_mode & S_IXGRP) || \
				 (st.st_mode & S_IXOTH)){
			cgi = 1;
		}else{
		}
		//method, path, cgi, query_string(GET)
		if(cgi){
			//cgi, path <- arg, POST() || GET(arg <- query_string) 
			printf("query_string: %s\n", query_string);
			exe_cgi(sock, path, method, query_string);
		}else{
			//not cgi, GET, NULL arg, path, size, st.st_size
			handler_header(sock);
			echo_www(sock, path, st.st_size);
		}
	}

end:
	if(err_code != 200){
		handler_header(sock);
		error_response(sock, err_code);
	}
	close(sock);
}









