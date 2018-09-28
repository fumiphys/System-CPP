/*
 * https server
 * author: fumiphys
 */

#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

const int IN_PORT = 3000;
const int BUF_SIZE = 1024;

int send_msg(SSL *ssl, const char *msg){
  int len = strlen(msg);
  if(SSL_write(ssl, msg, len) != len){
    perror("ssl write");
    std::cerr << "failed to write" << std::endl;
  }

  return len;
}

void https_res(SSL *ssl){
  int len;
  int read_fd = -1;

  char buf[1024];
  char method[1024];
  char uri_addr[256];
  char http_ver[64];
  char *uri_file;
  char default_file[11] = "index.html";

  /* read request */
  if(SSL_read(ssl, buf, BUF_SIZE - 1) <= 0){
    perror("ssl read");
    std::cerr << "failed to read" << std::endl;
  }

  /* get method, uri, version */
  sscanf(buf, "%s %s %s", method, uri_addr, http_ver);

  /* filter by Method */
  if(strcmp(method, "GET") == 0){
    uri_file = uri_addr + 1;
    if(strlen(uri_addr) == 1 && uri_addr[0] == '/'){
      uri_file = default_file;
    }

    if((read_fd = open(uri_file, O_RDONLY, 0666)) == -1){
      perror("open file");
      send_msg(ssl, "404 Not Found");
      close(read_fd);
      std::cerr << "GET: 404 Not Found" << std::endl;
      return;
    }

    /* send header */
    send_msg(ssl, "HTTP/1.0 200 OK\r\n");
    send_msg(ssl, "Content-Type: text/html\r\n");
    send_msg(ssl, "\r\n");

    /* send body */
    while((len = read(read_fd, buf, 1024)) > 0){
      if(send_msg(ssl, buf) != len){
        break;
      }
      memset(buf, 0, sizeof(buf));
    }

    std::cerr << "GET: 200" << std::endl;

  }else{
    if(strcmp(method, "GET") != 0){
      send_msg(ssl, "501 Not Implemented.");
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
  bool yes = 1;

  SSL_CTX *ctx;
  SSL *ssl;
  const char *crt_file = "https_server.crt";
  const char *key_file = "https_server.key";

  /* initialize */
  memset(&read_socket, 0, sizeof(read_socket));
  memset(&write_socket, 0, sizeof(write_socket));

  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();

  /* ssl */
  ctx = SSL_CTX_new(SSLv23_server_method());
  SSL_CTX_use_certificate_file(ctx, crt_file, SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM);

  /* address config */
  read_addr.sin_family = AF_INET;
  read_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  read_addr.sin_port = htons(IN_PORT);

  /* socket */
  read_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(read_socket < 0){
    perror("socket");
    std::cerr << "failed to make socket" << std::endl;
    exit(1);
  }

  setsockopt(read_socket, SOL_SOCKET, SO_REUSEADDR,
      (const char *)&yes, sizeof(yes));

  /* bind socket */
  if(bind(read_socket, (struct sockaddr *)&read_addr, sizeof(read_addr)) < 0){
    perror("bind");
    std::cerr << "failed to bind socket" << std::endl;
    exit(1);
  }

  /* listen */
  if(listen(read_socket, 5) < 0){
    perror("listen");
    std::cerr << "failed to listen" << std::endl;
    exit(1);
  }

  /* wait connection */
  while(1){
    if((write_socket = accept(read_socket, (struct sockaddr*)&write_addr, (socklen_t*)&write_len)) < 0){
      perror("accept");
      std::cerr << "failed to accept" << std::endl;
      exit(1);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, write_socket);
    int err;
    if((err = SSL_accept(ssl)) > 0){
      https_res(ssl);
    }else{
      ERR_print_errors_fp(stderr);
    }

    int sd = SSL_get_fd(ssl);
    SSL_free(ssl);
    close(sd);
    close(write_socket);
  }
  close(read_socket);
  SSL_CTX_free(ctx);

  return 0;
}
