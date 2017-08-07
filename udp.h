#pragma once


#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h> 
#include <Ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#include <stdint.h>

#include "part_defs.h"
#include "dgram_wrapper.h"

int init_udp_socket(int port, size_t index);
int kill_udp_socket(size_t index);
int send_dgram(struct dgram_wrapper *dgram, size_t index); /* Send datagram to ip:port */
int rx_dgram(struct dgram_wrapper *dgram, size_t index);    /* Receive datagram on bound socket and own the returning data ptr */

extern SOCKET udp_socket;