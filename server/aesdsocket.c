#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/fs.h>

#define PORT "9000" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define BUFSIZE 128
#define FILEPATH "/var/tmp/aesdsocketdata"


bool is_signal_caught = false;


static void sig_handler(int sig_num)
{
    if (sig_num == SIGINT || sig_num == SIGTERM) {
        is_signal_caught = true;
    }
}

int receive(int client_socket ) {
	char buf[BUFSIZE];
	int  err, ret, file;
	int rc = 0;
	
	// ready to communicate on socket descriptor
	memset(buf, 0, BUFSIZE);
	
	if((file = open(FILEPATH, O_CREAT | O_APPEND | O_RDWR, 0644)) == -1) {
		err = errno;
		perror("open error:");
		syslog(LOG_ERR, "open error: %s\n", strerror(err));
		rc = -1;
	}
	
	while((ret = recv(client_socket, buf, BUFSIZE-1, 0)) > 0) {
		
		buf[ret] = '\0';
		char *newline = strchr(buf, '\n');
		
		if (newline) {
			if(write(file, buf, newline - buf + 1) < 0) {
				err = errno;
				perror("write error:");
				syslog(LOG_ERR, "write error: %s\n", strerror(err));
				rc = -1;
				break;
			} else {
				
				syslog(LOG_DEBUG, "writing %s to %s", buf, FILEPATH);
				break;
			}
		}
		else {
            		if(write(file, buf, ret) < 0) {
				err = errno;
				perror("write error:");
				syslog(LOG_ERR, "write error: %s\n", strerror(err));
				break;
				rc = -1;
			} else {
				
				syslog(LOG_DEBUG, "writing %s to %s", buf, FILEPATH);
			}
		}

	}
	
	if (ret == -1) {
		err = errno;
		perror("recv error:");
		syslog(LOG_ERR, "recv error: %s\n", strerror(err));
		rc = -1;
	}
	
	lseek(file, 0, SEEK_SET);
	while ((ret = read(file, buf, BUFSIZE)) > 0) {
        	
        
        	if (send(client_socket, buf, ret, 0) < 0) {
			err = errno;
			perror("read error:");
			syslog(LOG_ERR, "read error: %s\n", strerror(err));
			rc = -1;
			break;
		}

    	}
	
	close(file);
	close(client_socket);
	return rc;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {

	int err = -1;
	int ret = -1;
	int sockfd = -1;
	struct addrinfo hints, *res;
	bool is_daemon_true = false;
	pid_t pid = -1;
	int client_socket = -1;
	int rc = 0;
	
	openlog(NULL, LOG_PID, LOG_USER);
	
	if(argc == 2) {
		if (strcmp(argv[1], "-d") == 0) {
			is_daemon_true = true;
		}
	}
	
	
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET; // use IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me
	
	if ((ret = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
		//fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(ret));
		syslog(LOG_ERR, "getaddrinfo error: %s\n", gai_strerror(ret));
		rc = -1;
	}
	
	
	// make a socket, bind it, and listen on it:
	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		err = errno;
		perror("socket error:");
		syslog(LOG_ERR, "socket error: %s\n", strerror(err));
		rc = -1;
	}
	
	int yes=1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
		err = errno;
		perror("setsockopt error:");
		syslog(LOG_ERR, "vs error: %s\n", strerror(err));
		rc = -1;
	}


	if ((ret = bind(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		err = errno;
		perror("bind error:");
		syslog(LOG_ERR, "bind error: %s\n", strerror(err));
		rc = -1;
		is_daemon_true = false;
	}
	
	if(is_daemon_true) {
		
		// create a new process
		pid = fork();
		if (pid == -1)
			rc = -1;
		else if (pid != 0) {
			
			freeaddrinfo(res); // free the linked-list
			close(sockfd);
			closelog();
		
			return rc;
		}
			
		// create a new session and process group
		rc = setsid();
			
		// set the working directory to the root directory
		rc = chdir("/") ;
			
		// redirect fd's 0,1,2 to /dev/null
		open("/dev/null", O_RDWR); 	// stdin
		dup(0); 			// stdout
		dup(0);			// stderror
				
	}
	
	if ((ret = listen(sockfd, BACKLOG)) == -1) {
		err = errno;
		perror("listen error:");
		syslog(LOG_ERR, "listen error: %s\n", strerror(err));
		rc = -1;
	}
	
	struct sigaction sig;
	sig.sa_handler = sig_handler;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);

	if (sigaction(SIGINT, &sig, NULL) == -1) {
		err = errno;
		perror("sigaction error:");
		syslog(LOG_ERR, "sigaction error: %s\n", strerror(err));
		rc = -1;
	}

	if (sigaction(SIGTERM, &sig, NULL) == -1) {
		err = errno;
		perror("sigaction error:");
		syslog(LOG_ERR, "sigaction error: %s\n", strerror(err));
		rc = -1;
	}

	
	while(!is_signal_caught) {
		struct sockaddr_storage their_addr;
		socklen_t addr_size;
		char ipstr[INET_ADDRSTRLEN ];
		
		// now accept an incoming connection
		addr_size = sizeof their_addr;
		if ((client_socket = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
			err = errno;
			perror("accept error:");
			syslog(LOG_ERR, "accept error: %s\n", strerror(err));
			rc = -1;
		} else {
				
			// convert the IP to a string
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)&their_addr;
			inet_ntop(res->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
			
				
			printf("Accepted connection from %s\n", ipstr);
			syslog(LOG_DEBUG, "Accepted connection from %s\n", ipstr);
					
					
			ret = receive(client_socket);
			close(client_socket);
			
			printf("Closed connection from %s\n", ipstr);
			syslog(LOG_DEBUG, "Closed connection from %s\n", ipstr);
	
					
			if(ret == -1) {
				printf("receive error\n");
				syslog(LOG_DEBUG, "receive error\n");
				rc = -1;
			}
		}
	}

	if(open(FILEPATH, O_CREAT | O_APPEND | O_RDWR, 0644) > 0) {
		remove(FILEPATH);
	}
	printf("Caught signal, exiting %d\n", pid);
	syslog(LOG_DEBUG, "Caught signal, exiting\n");
	
	freeaddrinfo(res); // free the linked-list
	
	shutdown(sockfd, SHUT_RDWR);
	close(client_socket);
	close(sockfd);
	closelog();
	
	printf("Done %d\n", pid);
	
	return rc;
}