/*
 * socket server
 * author: fumiphys
 */

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>

const int IN_PORT = 3000;

int send_msg(int fd, const char *msg){
  int len = strlen(msg);
  if(write(fd, msg, len) != len){
    std::cerr << "failed to write" << std::endl;
  }

  return len;
}

void http_res(int sockfd){
  int len;
  int read_fd = -1;

  char buf[1024];
  char method[1024];
  char uri_addr[256];
  char http_ver[64];
  char *uri_file;

  /* read request */
  if(read(sockfd, buf, 1024) <= 0){
    std::cerr << "failed to read" << std::endl;
  }

  /* get method, uri, version */
  sscanf(buf, "%s %s %s", method, uri_addr, http_ver);

  /* filter by Method */
  if(strcmp(method, "GET") == 0){
    uri_file = uri_addr + 1;
    if((read_fd = open(uri_file, O_RDONLY, 0666)) == -1){
      send_msg(sockfd, "404 Not Found");
      close(read_fd);
      std::cerr << "GET: 404 Not Found" << std::endl;
      return;
    }

    /* send header */
    send_msg(sockfd, "HTTP/1.0 200OK\r\n");
    send_msg(sockfd, "Content-Type: text/html\r\n");

    send_msg(sockfd, "\r\n");

    /* send body */
    while((len = read(read_fd, buf, 1024)) > 0){
      if(send_msg(sockfd, buf) != len){
        break;
      }
    }

    std::cerr << "GET: 200" << std::endl;

  }else{
    if(strcmp(method, "GET") != 0){
      send_msg(sockfd, "501 Not Implemented.");
      close(read_fd);
      std::cerr << "501 Not Implemented." << std::endl;
      return;
    }
  }

  close(read_fd);

}

int main(int argc, char const* argv[])
{
  int read_socket, write_socket;
  struct sockaddr_in read_addr, write_addr;
  int write_len;

  /* initialize */
  memset(&read_socket, 0, sizeof(read_socket));
  memset(&write_socket, 0, sizeof(write_socket));

  /* address config */
  read_addr.sin_family = AF_INET;
  read_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  read_addr.sin_port = htons(IN_PORT);

  /* socket */
  read_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(read_socket < 0){
    std::cerr << "failed to make socket" << std::endl;
    exit(1);
  }

  /* bind socket */
  if(bind(read_socket, (struct sockaddr *)&read_addr, sizeof(read_addr)) < 0){
    std::cerr << "failed to bind socket" << std::endl;
    exit(1);
  }

  /* listen */
  if(listen(read_socket, 5) < 0){
    std::cerr << "failed to listen" << std::endl;
    exit(1);
  }

  /* wait connection */
  while(1){
    if((write_socket = accept(read_socket, (struct sockaddr*)&write_addr, (socklen_t*)&write_len)) < 0){
      std::cerr << "failed to accept" << std::endl;
      exit(1);
    }

    http_res(write_socket);
    close(write_socket);
  }
  close(read_socket);

  return 0;
}
