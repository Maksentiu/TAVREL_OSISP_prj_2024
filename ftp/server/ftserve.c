#include "ftserve.h"


int main(int argc, char *argv[])
{	
	int sock_listen, sock_control, port, pid;

	if (argc != 2) {
		printf("usage: ./ftserve port\n");
		exit(0);
	}

	port = atoi(argv[1]);

	if ((sock_listen = socket_create(port)) < 0 ) {
		perror("Error creating socket");
		exit(1);
	}		
	
	while(1) {

		if ((sock_control = socket_accept(sock_listen))	< 0 )
			break;			
		
		if ((pid = fork()) < 0) { 
			perror("Error forking child process");
		} else if (pid == 0) { 
			close(sock_listen);
			ftserve_process(sock_control);		
			close(sock_control);
			exit(0);
		}
			
		close(sock_control);
	}

	close(sock_listen);	

	return 0;
}

void ftserve_retr(int sock_control, int sock_data, char* filename)
{	
	FILE* fd = NULL;
	char data[MAXSIZE];
	size_t num_read;							
		
	fd = fopen(filename, "r");
	
	if (!fd) {	
		send_response(sock_control, 550);
		
	} else {	
		send_response(sock_control, 150);
	
		do {
			num_read = fread(data, 1, MAXSIZE, fd);

			if (num_read < 0) {
				printf("error in fread()\n");
			}

			if (send(sock_data, data, num_read, 0) < 0)
				perror("error sending file\n");

		} while (num_read > 0);													
			
		send_response(sock_control, 226);

		fclose(fd);
	}
}


int server_put(int data_sock, char* arg)
{
    char data[MAXSIZE];
    int size;
    FILE* fd = fopen(arg, "w");
    
    while ((size = recv(data_sock, data, MAXSIZE, 0)) > 0) {
        fwrite(data, 1, size, fd);
    }

    if (size < 0) {
        perror("error\n");
    }

    fclose(fd);
    return 0;
}

int ftserve_list(int sock_data, int sock_control)
{
	char data[MAXSIZE];
	size_t num_read;									
	FILE* fd;

	int rs = system("ls -l | tail -n+2 > tmp.txt");
	if ( rs < 0) {
		exit(1);
	}
	
	fd = fopen("tmp.txt", "r");	
	if (!fd) {
		exit(1);
	}

	fseek(fd, SEEK_SET, 0);

	send_response(sock_control, 1);

	memset(data, 0, MAXSIZE);
	while ((num_read = fread(data, 1, MAXSIZE, fd)) > 0) {
		if (send(sock_data, data, num_read, 0) < 0) {
			perror("err");
		}
		memset(data, 0, MAXSIZE);
	}

	fclose(fd);

	send_response(sock_control, 226);

	return 0;	
}

int ftserve_start_data_conn(int sock_control)
{
	char buf[1024];	
	int wait, sock_data;

	if (recv(sock_control, &wait, sizeof wait, 0) < 0 ) {
		perror("Error while waiting");
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t len = sizeof client_addr;
	getpeername(sock_control, (struct sockaddr*)&client_addr, &len);
	inet_ntop(AF_INET, &client_addr.sin_addr, buf, sizeof(buf));

	if ((sock_data = socket_connect(CLIENT_PORT_ID, buf)) < 0)
		return -1;

	return sock_data;		
}

int ftserve_check_user(char*user, char*pass)
{
	char username[MAXSIZE];
	char password[MAXSIZE];
	char *pch;
	char buf[MAXSIZE];
	char *line = NULL;
	size_t num_read;									
	size_t len = 0;
	FILE* fd;
	int auth = 0;
	
	fd = fopen(".auth", "r");
	if (fd == NULL) {
		perror("file not found");
		exit(1);
	}	

	while ((num_read = getline(&line, &len, fd)) != -1) {
		memset(buf, 0, MAXSIZE);
		strcpy(buf, line);
		
		pch = strtok (buf," ");
		strcpy(username, pch);

		if (pch != NULL) {
			pch = strtok (NULL, " ");
			strcpy(password, pch);
		}

		trimstr(password, (int)strlen(password));

		if ((strcmp(user,username)==0) && (strcmp(pass,password)==0)) {
			auth = 1;
			break;
		}		
	}
	free(line);	
	fclose(fd);	
	return auth;
}

int ftserve_login(int sock_control)
{	
	char buf[MAXSIZE];
	char user[MAXSIZE];
	char pass[MAXSIZE];	
	memset(user, 0, MAXSIZE);
	memset(pass, 0, MAXSIZE);
	memset(buf, 0, MAXSIZE);
	
	if ( (recv_data(sock_control, buf, sizeof(buf)) ) == -1) {
		perror("recv error\n"); 
		exit(1);
	}	

	int i = 5;
	int n = 0;
	while (buf[i] != 0)
		user[n++] = buf[i++];
	
	send_response(sock_control, 331);					
	
	memset(buf, 0, MAXSIZE);
	if ( (recv_data(sock_control, buf, sizeof(buf)) ) == -1) {
		perror("recv error\n"); 
		exit(1);
	}
	
	i = 5;
	n = 0;
	while (buf[i] != 0) {
		pass[n++] = buf[i++];
	}
	
	return (ftserve_check_user(user, pass));
}

int ftserve_recv_cmd(int sock_control, char*cmd, char*arg)
{	
	int rc = 200;
	char buffer[MAXSIZE];
	
	memset(buffer, 0, MAXSIZE);
	memset(cmd, 0, 5);
	memset(arg, 0, MAXSIZE);
		
	// Wait to recieve command
	if ((recv_data(sock_control, buffer, sizeof(buffer)) ) == -1) {
		perror("recv error\n"); 
		return -1;
	}
	
	strncpy(cmd, buffer, 4);
	char *tmp = buffer + 5;
	strcpy(arg, tmp);
	
	if (strcmp(cmd, "QUIT")==0) {
		rc = 221;
	} else if((strcmp(cmd, "USER")==0) || (strcmp(cmd, "PASS")==0) ||
		    (strcmp(cmd, "LIST")==0) || (strcmp(cmd, "RETR")==0) || (strcmp(cmd, "STOR")==0)) {
		rc = 200;
	}  else {
		rc = 502;
	}

	send_response(sock_control, rc);	
	return rc;
}

void ftserve_process(int sock_control)
{
	int sock_data;
	char cmd[5];
	char arg[MAXSIZE];

	send_response(sock_control, 220);

	if (ftserve_login(sock_control) == 1) {
		send_response(sock_control, 230);
	} else {
		send_response(sock_control, 430);	
		exit(0);
	}	
	
	while (1) {
		int rc = ftserve_recv_cmd(sock_control, cmd, arg);
		
		if ((rc < 0) || (rc == 221)) {
			break;
		}
		
		if (rc == 200 ) {
			if ((sock_data = ftserve_start_data_conn(sock_control)) < 0) {
				close(sock_control);
				exit(1); 
			}

			if (strcmp(cmd, "LIST")==0) {
				ftserve_list(sock_data, sock_control);
			} else if (strcmp(cmd, "RETR")==0) {
				ftserve_retr(sock_control, sock_data, arg);
			}
			else if (strcmp(cmd, "STOR")==0) {
				server_put(sock_data, arg);
			}
		
			close(sock_data);
		} 
	}
}


