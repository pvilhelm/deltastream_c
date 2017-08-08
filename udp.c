#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "udp.h"
#include "clock.h"
#include "config.h"
#include "tinycthread\source\tinycthread.h"

#define DEFAULT_RX_BUFFER_SIZE 1600
#define DEFAULT_PORT 20000

SOCKET udp_socket[MAX_SIMULT_BROADCASTS*2]; /* Existing macro fucks up this */
uint16_t local_port[MAX_SIMULT_BROADCASTS*2];

static int inited;
mtx_t glb_lock;
//static mtx_t lcl_lock[];

int init_udp_lock() {
    return mtx_init(&glb_lock, mtx_plain);
}

int init_udp_socket(int port, size_t index) {

    mtx_lock(&glb_lock);

    if (index == SIZE_MAX) { /* Automatically choose index */
        for (int i = 0; i < sizeof udp_socket; i++) {
            if (udp_socket[i] == 0) {
                index = i;
                break; 
            }
        }
    }

    printf("Initializing udp socket ... \n");

    
    int res;
     
    struct sockaddr_in rx_address;

    /* Initialize Winsock */
    if (!inited) {
        WSADATA wsaData;
        res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) {
            printf("WSAStartup failed with error: %d\n", res);
            return mtx_unlock(&glb_lock), -1;
        }
        inited = 1;
    } else { 
        inited++; 
    }

    /* Create UDP socket */
    udp_socket[index] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket[index] == INVALID_SOCKET) {
        int errn = WSAGetLastError();
        printf("socket failed with error: %ld\n", errn); 
        WSACleanup();
        return mtx_unlock(&glb_lock), -1;
    }

    rx_address.sin_family = AF_INET;
    rx_address.sin_port = htons(port);
    rx_address.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bind socket to any local ip */
    res = bind((SOCKET)udp_socket[index], (SOCKADDR*) &rx_address, sizeof rx_address);

    if (res == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError()); 
        closesocket((SOCKET)udp_socket[index]);
        WSACleanup();
        return mtx_unlock(&glb_lock), -1;
    }

    local_port[index] = port; 

    printf("Udp socket initialized! \n");
    return mtx_unlock(&glb_lock), index;
}

int kill_udp_socket(size_t index) {
    mtx_lock(&glb_lock);
    printf("Closing udp socket ... \n");
    closesocket((SOCKET)udp_socket[index]);
    udp_socket[index] = 0; 
    local_port[index] = 0;
    if (!--inited)
        return mtx_unlock(&glb_lock), WSACleanup();
    return mtx_unlock(&glb_lock), 0;
}

int send_dgram(struct dgram_wrapper *dgram, size_t index)
{ 
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(dgram->remote_ip);
    dest.sin_port = htons(dgram->port);

    int res = sendto((SOCKET)udp_socket[index], dgram->data, (int)dgram->data_length, 0, (SOCKADDR *)&dest, sizeof(dest));
    
    return res; 
}

int rx_dgram(struct dgram_wrapper *dgram, size_t index)
{

    struct sockaddr_in from = { 0 };
    int from_size = sizeof from;

    dgram->data = calloc(DEFAULT_RX_BUFFER_SIZE, sizeof(char));
    int res = recvfrom((SOCKET)udp_socket[index], dgram->data, DEFAULT_RX_BUFFER_SIZE, 0, (SOCKADDR*) &from, &from_size);
    dgram->time = get_time_deci_ms();
    dgram->remote_ip = ntohl(from.sin_addr.s_addr);
    dgram->port = ntohs(from.sin_port);
    dgram->local_port = ntohs(local_port[index]);
     
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
