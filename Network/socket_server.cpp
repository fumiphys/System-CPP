/*
 * socket server
 * author: fumiphys
 */

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int IN_PORT = 3000;


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

    write(write_socket, "HTTP1.1 200 OK", 14);
    close(write_socket);
  }
  close(read_socket);

  return 0;
}
