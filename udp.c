#include <stdio.h>
#include <stdlib.h>

#include "udp.h"

#define DEFAULT_RX_BUFFER_SIZE 1600
#define DEFAULT_PORT 20000

SOCKET udp_socket = INVALID_SOCKET;

int init_udp_socket(int port) {

    printf("Initializing udp socket ... \n");

    WSADATA wsaData;
    int res;
     
    struct sockaddr_in rx_address;

    /* Initialize Winsock */
    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        printf("WSAStartup failed with error: %d\n", res);
        return 1;
    }

    /* Create UDP socket */
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket == INVALID_SOCKET) {
        int errn = WSAGetLastError();
        printf("socket failed with error: %ld\n", errn); 
        WSACleanup();
        return 1;
    }

    rx_address.sin_family = AF_INET;
    rx_address.sin_port = htons(port);
    rx_address.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bind socket to any local ip */
    res = bind(udp_socket, (SOCKADDR*) &rx_address, sizeof rx_address);

    if (res == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError()); 
        closesocket(udp_socket);
        WSACleanup();
        return 1;
    }

    printf("Udp socket initialized! \n");
    return 0; 
}

int kill_udp_socket() {
    printf("Closing udp socket ... \n");
    closesocket(udp_socket);
    return WSACleanup();
}

int send_dgram(dgram_wrapper_t dgram)
{ 
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(dgram.remote_ip);
    dest.sin_port = htons(dgram.port);

    int res = sendto(udp_socket, dgram.data, (int)dgram.data_length, 0, (SOCKADDR *)&dest, sizeof(dest));
    
    return res; 
}

int rx_dgram(dgram_wrapper_t* dgram)
{

    struct sockaddr_in from = { 0 };
    int from_size = sizeof from;

    dgram->data = calloc(DEFAULT_RX_BUFFER_SIZE, sizeof(char));
    int res = recvfrom(udp_socket, dgram->data, DEFAULT_RX_BUFFER_SIZE, 0, (SOCKADDR*) &from, &from_size);

    dgram->remote_ip = ntohl(from.sin_addr.s_addr);
    dgram->port = ntohs(from.sin_port);
     
    if (res == SOCKET_ERROR) {
        wchar_t *s = NULL;
        int errn = WSAGetLastError();
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errn,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, NULL);
        fprintf(stderr, "%S\n", s);
        LocalFree(s);
        return errn;
    } else {
        dgram->data_length = res;
        return 0;
    }
}
