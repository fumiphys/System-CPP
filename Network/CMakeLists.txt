cmake_minimum_required(VERSION 2.8)

find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "failed to find OpenSSL")
endif()
message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(socket_server socket_server.cpp)
add_executable(http_server http_server.cpp)
add_executable(http_client http_client.cpp)
add_executable(https_client https_client.cpp)
add_executable(https_server https_server.cpp)

target_link_libraries(https_client OpenSSL::SSL)
target_link_libraries(https_client OpenSSL::Crypto)
target_link_libraries(https_server OpenSSL::SSL)
target_link_libraries(https_server OpenSSL::Crypto)
