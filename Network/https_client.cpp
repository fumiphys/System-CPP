/*
 * http client
 * author: fumiphys
 */

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const int BUF_SIZE = 1024;

int send_msg(SSL *ssl, const char *msg){
  int len = strlen(msg);
  if(SSL_write(ssl, msg, len) != len){
    perror("ssl write");
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

  SSL *ssl;
  SSL_CTX *ctx;

  /* initialize */
  memset(&write_socket, 0, sizeof(write_socket));

  /* address config */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  const char *service = "3000";

  if(getaddrinfo(host, service, &hints, &res) != 0){
    perror("get address info");
    std::cerr << "failed to get address info." << std::endl;
    exit(1);
  }

  /* socket */
  write_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol < 0);
  if(write_socket < 0){
    perror("socket");
    std::cerr << "failed to make socket" << std::endl;
    exit(1);
  }

  /* connect */
  if(connect(write_socket, res->ai_addr, res->ai_addrlen) != 0){
    perror("connect");
    std::cerr << "failed to connect to host." << std::endl;
    exit(1);
  }

  /* ssl */
  SSL_load_error_strings();
  SSL_library_init();

  ctx = SSL_CTX_new(SSLv23_client_method());
  ssl = SSL_new(ctx);
  if(ssl == NULL){
    ERR_print_errors_fp(stderr);
    exit(1);
  }
  SSL_set_fd(ssl, write_socket);
  SSL_connect(ssl);

  /* send message */
  char msg[100];
  sprintf(msg, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
  send_msg(ssl, msg);

  /* receive response */
  while(1){
    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));
    int read_size = SSL_read(ssl, buf, BUF_SIZE - 1);

    if(read_size > 0){
      printf("%s", buf);
    }else{
      break;
    }
  }

  SSL_shutdown(ssl);
  SSL_free(ssl);
  SSL_CTX_free(ctx);
  ERR_free_strings();

  freeaddrinfo(res);
  close(write_socket);

  return 0;
}
