#ifndef __NETWORK_UTIL_H__
#define __NETWORK_UTIL_H__

#ifdef LINUX
#include <stdint.h>
#include <stdio.h>
#endif

#if !defined(LINUX)
# define __LITTLE_ENDIAN 1234
# define BYTE_ORDER LITTLE_ENDIAN
#else
# include <arpa/inet.h>
#endif

//Define ntohs and htons (2 byte)
#if !defined(ntohs) && (BYTE_ORDER == LITTLE_ENDIAN)
# define ntohs(n) ((((int16_t)(n)) & 0xff00) >> 8 | (((int16_t)(n)) & 0xff) << 8)
# define htons(n) ntohs(n)
#elif !defined(ntohs)
# define ntohs(n) ((int16_t)(n))
# define htons(n) ntohs(n)
#endif

//Define ntohl and htonl (4 byte)
#if !defined(ntohl) && (BYTE_ORDER == LITTLE_ENDIAN)
# define ntohl(x) ((((x)&0xff000000)>>24) \
                  |(((x)&0x00ff0000)>>8)  \
                  |(((x)&0x0000ff00)<<8)  \
                  |(((x)&0x000000ff)<<24))
# define htonl(n) ntohl(n)
#elif !defined(ntohl)
# define ntohl(n) ((int32_t)(n))
# define htonl(n) ntohl(n)
#endif

//Define ntohll and htonll (8 byte)
#if !defined(ntohll)
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

typedef uint16_t (*NetworkWriter)(void *userdata, uint8_t *data, uint16_t length);
typedef int16_t (*NetworkReader)(void *userdata);

#endif
