#ifndef MOCK_IP_ADDR_H
#define MOCK_IP_ADDR_H

#include <stdint.h>
#include <arpa/inet.h>

typedef struct {
    uint32_t addr;
} ip4_addr_t;

typedef struct {
    ip4_addr_t u_addr;
} ip_addr_t;

#define ip_2_ip4(ipaddr) (&((ipaddr)->u_addr))
#define ip4_addr_get_u32(ipaddr) ((ipaddr)->addr)

static inline int ipaddr_aton(const char *cp, ip_addr_t *addr) {
    return inet_pton(AF_INET, cp, &addr->u_addr.addr);
}

#endif
