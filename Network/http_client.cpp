/*
 * http client
 * author: fumiphys
 */

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>

const int BUF_SIZE = 1024;

int send_msg(int fd, const char *msg){
  int len = strlen(msg);
  if(write(fd, msg, len) != len){
    std::cerr << "failed to write" << std::endl;
  }

  return len;
}

int main(int argc, char const* argv[])
{
  int write_socket;
  struct addrinfo hints, *res;

  const char *host = "127.0.0.1";
  const char *path = "/";

  /* initialize */
  memset(&write_socket, 0, sizeof(write_socket));

  /* address config */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  char service[6] = "3000";

  if(getaddrinfo(host, service, &hints, &res) != 0){
    std::cerr << "failed to get address info." << std::endl;
    exit(1);
  }

  /* socket */
  write_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol < 0);
  if(write_socket < 0){
    std::cerr << "failed to make socket" << std::endl;
    exit(1);
  }

  /* connect */
  if(connect(write_socket, res->ai_addr, res->ai_addrlen) != 0){
    std::cerr << "failed to connect to host." << std::endl;
    exit(1);
  }

  /* send message */
  char msg[100];
  sprintf(msg, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
  send_msg(write_socket, msg);

  /* receive response */
  while(1){
    char buf[BUF_SIZE];
    int read_size;
    read_size = read(write_socket, buf, BUF_SIZE);

    if(read_size > 0){
      printf("%s", buf);
    }else{
      break;
    }
  }

  close(write_socket);

  return 0;
}
