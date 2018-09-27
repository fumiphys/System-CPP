#!/bin/bash -eu
# create key and ctr file
openssl genrsa 2048 > https_server.key
openssl req -new -key https_server.key > https_server.csr
openssl x509 -days 3650 -req -signkey https_server.key < https_server.csr > https_server.crt
