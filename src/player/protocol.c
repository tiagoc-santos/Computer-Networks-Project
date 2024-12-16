#include "./headers/protocol.h"

int get_server_info(struct addrinfo** res, char* server_IP, char* server_port, int udp){
    int errcode;
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = udp ? SOCK_DGRAM : SOCK_STREAM;

    if ((errcode = getaddrinfo(server_IP, server_port, &hints, res)) != 0){
        fprintf(stderr,"error: getaddrinfo: %s\n",gai_strerror(errcode));
        return -1;
    }
    return 0;
}

int create_socket(int udp){
    int socket_fd;
    struct timeval timeout;

    if (udp){
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0); //UDP socket
    }
    else{
        socket_fd = socket(AF_INET, SOCK_STREAM, 0); //TCP socket 
    }

    if (socket_fd == -1){
        close(socket_fd);
        return -1;
    }

    /*Set timeout for operations*/
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        fprintf(stderr, "Error setting timeout.\n %s\n", strerror(errno));
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

/*------------------------- UDP Requests -------------------------*/

int send_message_udp(int socket_fd, struct addrinfo* res, char* message, int message_size){
    size_t bytes_sent;

    bytes_sent = sendto(socket_fd, message, message_size, 0, res->ai_addr, res->ai_addrlen);
    if (bytes_sent == -1){
        fprintf(stderr, "Error sending message.\n");
        return -1;
    }
    return 0;
}

int receive_message_udp(int socket_fd, struct addrinfo* res, char* buffer){
    size_t bytes_received;
    struct sockaddr_in addr;
    socklen_t addrlen;
    
    addrlen = sizeof(addr);

    bytes_received = recvfrom(socket_fd, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &addrlen);
    
    if (bytes_received == -1){
        // Checks for timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            return TIMEOUT;
        }
        else{
            return -1;
        }
    }
    buffer[bytes_received] = '\0';
    return 0;
}

int send_udp_request(char* message, int message_size, int socket_fd, struct addrinfo* res, char* buffer){
    int n = 0, ret;
    
    while(n < MAX_RESENDS){
        if (send_message_udp(socket_fd, res, message, message_size) == -1) {
            fprintf(stderr, "Error sending message.\n");
            return -1;
        }
        ret = receive_message_udp(socket_fd, res, buffer);

        // If connection timeouts
        if (ret != 0){
            if (ret == TIMEOUT){
                fprintf(stderr, "TIMEOUT trying again...\n");
                n++;
                continue;
            }
            // Other unknown error occured
            else{
                fprintf(stderr, "Unknown error.\n");
                return -1;
            }
        }
        else{
            return 0;
        }
    }
    fprintf(stderr, "Could not reach the server. Try again later.\n");
    return -1;
}

/*------------------------- TCP Requests -------------------------*/

int connect_server(int tcp_socket, struct addrinfo* res){
    if(connect(tcp_socket, res->ai_addr, res->ai_addrlen) == -1){
        fprintf(stderr, "Error occured while connecting to server.\n");
        if(close(tcp_socket) == -1){
            fprintf(stderr, "Error closing socket.\n");
        }
        return -1;
    }

    return 0;
}

int write_message_tcp(int tcp_socket, char* message){
    char* ptr, buffer[MSG_SIZE];
    int nbytes = strlen(message), nwritten;
    int nleft = nbytes;
    ptr = strcpy(buffer, message);
    while(nleft > 0){
        nwritten = write(tcp_socket,ptr,nleft);
        if(nwritten == -1){
            fprintf(stderr, "Error sending the message.\n");
            if(close(tcp_socket) == -1){
                fprintf(stderr, "Error closing socket.\n");
            }
            return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    return 0;
}

int read_message_tcp(int tcp_socket, char buffer[BUFFER_SIZE],int size){
    memset(buffer, 0, BUFFER_SIZE);
    int aux = read(tcp_socket, buffer, size);
    if (aux == -1) {
        // check for a timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            fprintf(stderr,"Timeout: Server did not respond within the specified time.\n");
        
        else 
            fprintf(stderr, "Receive failed.\n");
        
        if(close(tcp_socket) == -1)
            fprintf(stderr, "Error closing server connection.\n");
        
        return -1;
    }
    
    return aux;
}

int read_file_tcp(int tcp_socket, char filename[FILENAME_SIZE]){
    char buffer[BUFFER_SIZE], response[BUFFER_SIZE], 
    filesize[4], filepath[BUFFER_SIZE];
    int file;
    int i = 0, k = 0, j = 0, l = 0;

    while (1){
        if (read_message_tcp(tcp_socket, buffer, 1) == -1) {
            fprintf(stderr, "Error reading file from server\n");
            return -1;
        }
        // Server responded with ERR
        if (buffer[0] == 'E' && k == 0) {
            fprintf(stderr, "Unexpected message received.\n");
            return -1;
        }

        if (buffer[0] == ' ' || buffer[0] == '\n'){
            k++;

            if(k == 1)
                response[i++] = buffer[0];
            if(k == 2)
                response[i] = '\0';

            if(k == 2 && !strcmp(response, "RST NOK")){
                fprintf(stdout, "There is no games in the player record.\n");
                return 0;
            }

            if(k == 2 && !strcmp(response, "RSS EMPTY")){
                return 1;
            }

            else if(k > 3){
                filesize[l] = '\0';
                break;
            }

            continue;
        }

        if(k < 2)
            response[i++] = buffer[0];
        
        if(k == 2)
            filename[j++] = buffer[0];
        
        if(k == 3){
            filename[j] = '\0';
            filesize[l++] = buffer[0]; 
        }
    }

    strcpy(filepath, "src/player/scores/");
    // Creates directory for scores and trials
    if (mkdir(filepath, 0777) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr,"Error creating directory");
            return -1;
        }
    }

    strcat(filepath, filename);
    file = open(filepath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    int fsize = atoi(filesize);
    if(file == -1){
        fprintf(stderr, "Error creating file.\n");
        return -1;
    }
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = 0;

    while(1){
        int bytes_written = 0;
        bytes_read = read(tcp_socket, buffer, BUFFER_SIZE);
        if (bytes_read == -1){
            fprintf(stderr, "Error reading file data.\n");
            return -1;
        }
        if (bytes_read == 0 || fsize == 0)
            break;

        int aux = 0;
        while (bytes_written < bytes_read){
            bytes_written = write(file, buffer, bytes_read);
            if (bytes_written == -1){
                fprintf(stderr, "Error writing file data.\n");
                return -1;
            }
            bytes_written += aux;
        }
    }

    if(close(file) == -1){
        fprintf(stderr, "Error closing the file.\n");
        return -1;
    }
    return fsize;
}

int send_tcp_request(char message[MSG_SIZE], char filename[FILENAME_SIZE]){
    int tcp_socket;

    if ((tcp_socket = create_socket(0)) == -1) {
        fprintf(stderr, "Error establishing TCP connection");
        return -1;
    }

    struct addrinfo* server_info_tcp;
    if (get_server_info(&server_info_tcp, server_IP, server_port, 0) != 0)
        return -1;

    if(connect_server(tcp_socket, server_info_tcp) != 0)
        return -1;

    if(write_message_tcp(tcp_socket, message) != 0)
        return -1;

    int status = read_file_tcp(tcp_socket, filename);
    if(close(tcp_socket) != 0){
        fprintf(stderr, "Error closing tcp connection.\n");
        return -1;
    }

    return status;
}