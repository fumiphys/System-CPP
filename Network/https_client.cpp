/*
 * http client
 * author: fumiphys
 */

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const int TO_PORT = 3000;
const int BUF_SIZE = 1024;

int send_msg(SSL *ssl, const char *msg){
  int len = strlen(msg);
  if(SSL_write(ssl, msg, len) != len){
    std::cerr << "failed to write" << std::endl;
  }

  return len;
}

int main(int argc, char const* argv[])
{
  int write_socket;
  struct sockaddr_in write_addr;

  SSL *ssl;
  SSL_CTX *ctx;

  /* initialize */
  memset(&write_socket, 0, sizeof(write_socket));

  /* address config (to: 127.0.0.1) */
  write_addr.sin_family = AF_INET;
  write_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  write_addr.sin_port = htons(TO_PORT);

  /* socket */
  write_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(write_socket < 0){
    std::cerr << "failed to make socket" << std::endl;
    exit(1);
  }

  /* connect */
  if(connect(write_socket, (struct sockaddr*)&write_addr, sizeof(struct sockaddr))){
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
  int err = SSL_set_fd(ssl, write_socket);
  SSL_connect(ssl);

  /* send message */
  send_msg(ssl, "GET / HTTP/1.1\r\n");
  send_msg(ssl, "Content-Length: 0\r\n");
  send_msg(ssl, "\r\n");

  /* receive response */
  while(1){
    char buf[BUF_SIZE];
    int read_size;
    read_size = SSL_read(ssl, buf, BUF_SIZE);

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

  close(write_socket);

  return 0;
}
