#ifndef _BRIDGE_H_
#define _BRIDGE_H_

#include "lwip/netif.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Layer-2 pseudo-bridge between STA and AP netifs.
 * 
 * Installs packet hooks on both netifs, enables Proxy ARP, and DHCP snooping.
 * 
 * @param sta_netif lwIP netif for Station (uplink) interface
 * @param ap_netif lwIP netif for Access Point (downlink) interface
 */
void bridge_init(struct netif *sta_netif, struct netif *ap_netif);

/**
 * @brief Deinitialize and restore original netif hooks.
 */
void bridge_deinit(void);

/**
 * @brief Check if Layer-2 bridge is currently active.
 */
bool bridge_is_active(void);

/**
 * @brief Print the Forwarding Database (FDB) entries.
 */
void bridge_show_fdb(void);

#ifdef __cplusplus
}
#endif

#endif /* _BRIDGE_H_ */
