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

int init_udp_socket(int port);
int kill_udp_socket();
int send_dgram(dgram_wrapper_t dgram); /* Send datagram to ip:port */
int rx_dgram(dgram_wrapper_t* dgram);    /* Receive datagram on bound socket and own the returning data ptr */

extern SOCKET udp_socket;