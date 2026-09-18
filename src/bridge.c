#include "bridge.h"

#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "lwip/def.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
#include "lwip/ip4_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

static const char *TAG = "Bridge";

/* -------------------------------------------------------------------------
 * Netif state & original handlers
 * ------------------------------------------------------------------------- */
static struct netif        *s_sta_nif = NULL;
static struct netif        *s_ap_nif = NULL;
static netif_input_fn       s_orig_input_sta = NULL;
static netif_input_fn       s_orig_input_ap = NULL;
static netif_output_fn      s_orig_output_sta = NULL;
static netif_output_fn      s_orig_output_ap = NULL;
static netif_linkoutput_fn  s_orig_lo_sta = NULL;
static netif_linkoutput_fn  s_orig_lo_ap = NULL;
static volatile bool        s_bridge_enabled = false;

static portMUX_TYPE         s_bridge_mux = portMUX_INITIALIZER_UNLOCKED;

/* -------------------------------------------------------------------------
 * Compact packed header types
 * ------------------------------------------------------------------------- */
#define ETHTYPE_IP  0x0800
#define ETHTYPE_ARP 0x0806

typedef struct {
    uint8_t  dst[6];
    uint8_t  src[6];
    uint16_t type;
} __attribute__((packed)) eth_hdr_t;

typedef struct {
    uint16_t hwtype;
    uint16_t prtype;
    uint8_t  hwlen;
    uint8_t  prlen;
    uint16_t op;
    uint8_t  sha[6];
    uint32_t spa;
    uint8_t  tha[6];
    uint32_t tpa;
} __attribute__((packed)) arp_hdr_t;

typedef struct {
    uint8_t  vhl;
    uint8_t  tos;
    uint16_t len;
    uint16_t id;
    uint16_t off;
    uint8_t  ttl;
    uint8_t  proto;
    uint16_t chksum;
    uint32_t src;
    uint32_t dst;
} __attribute__((packed)) ip_hdr_t;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t chksum;
} __attribute__((packed)) udp_hdr_t;

#define DHCP_OP_REQUEST   1
#define DHCP_OP_REPLY     2
#define DHCP_MSG_DISCOVER 1
#define DHCP_MSG_REQUEST  3
#define DHCP_MSG_ACK      5
#define DHCP_MAGIC_COOKIE 0x63825363UL

typedef struct {
    uint8_t  op;
    uint8_t  htype;
    uint8_t  hlen;
    uint8_t  hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t  chaddr[16];
    uint8_t  sname[64];
    uint8_t  file[128];
    uint32_t magic;
    uint8_t  options[0];
} __attribute__((packed)) dhcp_msg_t;

/* -------------------------------------------------------------------------
 * Time helper
 * ------------------------------------------------------------------------- */
static inline uint32_t now_secs(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000000ULL);
}

/* -------------------------------------------------------------------------
 * Forwarding Database (FDB)
 * ------------------------------------------------------------------------- */
#define FDB_SIZE   32
#define FDB_TTL_S  600

typedef struct {
    uint32_t ip;
    uint8_t  mac[6];
    uint32_t expires_s;
} fdb_entry_t;

static fdb_entry_t s_fdb[FDB_SIZE];

static void fdb_insert(uint32_t ip, const uint8_t *mac)
{
    if (ip == 0) return;
    if (s_sta_nif && ip == netif_ip4_addr(s_sta_nif)->addr) return;
    if (s_ap_nif  && ip == netif_ip4_addr(s_ap_nif)->addr)  return;

    uint32_t now = now_secs();
    int free_idx = -1, oldest_idx = 0;
    uint32_t oldest_exp = 0xFFFFFFFFUL;

    portENTER_CRITICAL(&s_bridge_mux);
    for (int i = 0; i < FDB_SIZE; i++) {
        if (s_fdb[i].ip == ip) {
            memcpy(s_fdb[i].mac, mac, 6);
            s_fdb[i].expires_s = now + FDB_TTL_S;
            portEXIT_CRITICAL(&s_bridge_mux);
            return;
        }
        if (s_fdb[i].ip == 0 || s_fdb[i].expires_s <= now) {
            if (free_idx < 0) free_idx = i;
        }
        if (s_fdb[i].expires_s < oldest_exp) {
            oldest_exp = s_fdb[i].expires_s;
            oldest_idx = i;
        }
    }
    int idx = (free_idx >= 0) ? free_idx : oldest_idx;
    s_fdb[idx].ip = ip;
    memcpy(s_fdb[idx].mac, mac, 6);
    s_fdb[idx].expires_s = now + FDB_TTL_S;
    portEXIT_CRITICAL(&s_bridge_mux);
}

static bool fdb_lookup(uint32_t ip, uint8_t out_mac[6])
{
    uint32_t now = now_secs();
    portENTER_CRITICAL(&s_bridge_mux);
    for (int i = 0; i < FDB_SIZE; i++) {
        if (s_fdb[i].ip == ip && s_fdb[i].expires_s > now) {
            memcpy(out_mac, s_fdb[i].mac, 6);
            portEXIT_CRITICAL(&s_bridge_mux);
            return true;
        }
    }
    portEXIT_CRITICAL(&s_bridge_mux);
    return false;
}

/* -------------------------------------------------------------------------
 * DHCP XID map
 * ------------------------------------------------------------------------- */
#define XID_MAP_SIZE  16
#define XID_TTL_S     30

typedef struct {
    uint32_t xid;
    uint8_t  chaddr[6];
    uint32_t expires_s;
} xid_entry_t;

static xid_entry_t s_xid_map[XID_MAP_SIZE];

static void xid_map_insert(uint32_t xid, const uint8_t *chaddr)
{
    uint32_t now = now_secs();
    int free_idx = -1, oldest_idx = 0;
    uint32_t oldest_exp = 0xFFFFFFFFUL;

    portENTER_CRITICAL(&s_bridge_mux);
    for (int i = 0; i < XID_MAP_SIZE; i++) {
        if (s_xid_map[i].xid == xid) {
            memcpy(s_xid_map[i].chaddr, chaddr, 6);
            s_xid_map[i].expires_s = now + XID_TTL_S;
            portEXIT_CRITICAL(&s_bridge_mux);
            return;
        }
        if (s_xid_map[i].xid == 0 || s_xid_map[i].expires_s <= now) {
            if (free_idx < 0) free_idx = i;
        }
        if (s_xid_map[i].expires_s < oldest_exp) {
            oldest_exp = s_xid_map[i].expires_s;
            oldest_idx = i;
        }
    }
    int idx = (free_idx >= 0) ? free_idx : oldest_idx;
    s_xid_map[idx].xid = xid;
    memcpy(s_xid_map[idx].chaddr, chaddr, 6);
    s_xid_map[idx].expires_s = now + XID_TTL_S;
    portEXIT_CRITICAL(&s_bridge_mux);
}

static bool xid_map_lookup(uint32_t xid, uint8_t out_chaddr[6])
{
    uint32_t now = now_secs();
    portENTER_CRITICAL(&s_bridge_mux);
    for (int i = 0; i < XID_MAP_SIZE; i++) {
        if (s_xid_map[i].xid == xid && s_xid_map[i].expires_s > now) {
            memcpy(out_chaddr, s_xid_map[i].chaddr, 6);
            portEXIT_CRITICAL(&s_bridge_mux);
            return true;
        }
    }
    portEXIT_CRITICAL(&s_bridge_mux);
    return false;
}

/* -------------------------------------------------------------------------
 * Packet Inspection Helpers
 * ------------------------------------------------------------------------- */
static inline void *pkt_at(struct pbuf *p, uint16_t off, uint16_t need)
{
    if (p->len < (uint16_t)(off + need)) return NULL;
    return (uint8_t *)p->payload + off;
}

static void update_ip_chksum(ip_hdr_t *ip)
{
    ip->chksum = 0;
    uint32_t sum = 0;
    uint8_t ihl = (ip->vhl & 0x0f);
    if (ihl < 5) ihl = 5;
    const uint8_t *raw = (const uint8_t *)ip;
    for (int i = 0; i < ihl * 2; i++) {
        uint16_t word;
        memcpy(&word, &raw[i * 2], sizeof(word));
        sum += word;
    }
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    ip->chksum = ~((uint16_t)sum);
}

static uint8_t *dhcp_find_option(uint8_t *opts, uint16_t opts_len, uint8_t tag, uint8_t *out_len)
{
    uint8_t *p = opts;
    uint8_t *end = opts + opts_len;
    while (p < end) {
        uint8_t t = *p;
        if (t == 255) { if (tag == 255) return p; break; }
        if (t == 0)  { p++; continue; }
        uint8_t l = (p + 1 < end) ? *(p + 1) : 0;
        if (t == tag) { if (out_len) *out_len = l; return p + 2; }
        p += 2 + l;
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * Proxy ARP
 * ------------------------------------------------------------------------- */
static void send_proxy_arp_reply(struct netif *tx_nif, netif_linkoutput_fn lo, const arp_hdr_t *req, uint32_t target_ip)
{
    uint16_t pkt_len = sizeof(eth_hdr_t) + sizeof(arp_hdr_t);
    struct pbuf *p = pbuf_alloc(PBUF_RAW, pkt_len, PBUF_RAM);
    if (!p) return;
    eth_hdr_t *eth = (eth_hdr_t *)p->payload;
    arp_hdr_t *arp = (arp_hdr_t *)((uint8_t *)p->payload + sizeof(eth_hdr_t));
    memcpy(eth->dst, req->sha, 6);
    memcpy(eth->src, tx_nif->hwaddr, 6);
    eth->type = htons(ETHTYPE_ARP);
    arp->hwtype = req->hwtype;
    arp->prtype = req->prtype;
    arp->hwlen = req->hwlen;
    arp->prlen = req->prlen;
    arp->op = htons(2); // Reply
    memcpy(arp->sha, tx_nif->hwaddr, 6);
    arp->spa = target_ip;
    memcpy(arp->tha, req->sha, 6);
    arp->tpa = req->spa;
    lo(tx_nif, p);
    pbuf_free(p);
}

/* -------------------------------------------------------------------------
 * DHCP Snooping
 * ------------------------------------------------------------------------- */
static bool snoop_dhcp_request(struct pbuf *p, uint16_t eth_ip_udp_hdr_len)
{
    dhcp_msg_t *dhcp = (dhcp_msg_t *)pkt_at(p, eth_ip_udp_hdr_len, sizeof(dhcp_msg_t));
    if (!dhcp || dhcp->op != DHCP_OP_REQUEST || dhcp->hlen != 6 || dhcp->magic != htonl(DHCP_MAGIC_COOKIE)) return false;

    uint8_t client_mac[6];
    memcpy(client_mac, dhcp->chaddr, 6);
    memcpy(dhcp->chaddr, s_sta_nif->hwaddr, 6);
    dhcp->flags |= htons(0x8000); // Request broadcast response

    uint16_t opts_len = p->len - eth_ip_udp_hdr_len - (uint16_t)sizeof(dhcp_msg_t);
    uint8_t msg_type = 0, optlen = 0;
    uint8_t *opt = dhcp_find_option(dhcp->options, opts_len, 53, &optlen);
    if (opt && optlen >= 1) msg_type = opt[0];

    bool opt61_added = false;
    if (msg_type == DHCP_MSG_DISCOVER || msg_type == DHCP_MSG_REQUEST) {
        xid_map_insert(dhcp->xid, client_mac);
        uint8_t opt61len = 0;
        uint8_t *opt61 = dhcp_find_option(dhcp->options, opts_len, 61, &opt61len);
        if (!opt61 && opts_len + 9 <= 308) {
            uint8_t *end = dhcp_find_option(dhcp->options, opts_len, 255, NULL);
            if (end && (end + 10 <= (uint8_t *)dhcp->options + opts_len + 16)) {
                end[0] = 61; end[1] = 7; end[2] = 1;
                memcpy(&end[3], client_mac, 6);
                end[9] = 255;
                uint16_t *ip_len_ptr = (uint16_t *)pkt_at(p, sizeof(eth_hdr_t) + 2, 2);
                uint16_t *udp_len_ptr = (uint16_t *)pkt_at(p, eth_ip_udp_hdr_len - 4, 2);
                if (ip_len_ptr) *ip_len_ptr = htons(ntohs(*ip_len_ptr) + 9);
                if (udp_len_ptr) *udp_len_ptr = htons(ntohs(*udp_len_ptr) + 9);
                p->len += 9;
                p->tot_len += 9;
                ip_hdr_t *ip = (ip_hdr_t *)pkt_at(p, sizeof(eth_hdr_t), sizeof(ip_hdr_t));
                if (ip) update_ip_chksum(ip);
                opt61_added = true;
            }
        }
        udp_hdr_t *udp = (udp_hdr_t *)pkt_at(p, eth_ip_udp_hdr_len - (uint16_t)sizeof(udp_hdr_t), sizeof(udp_hdr_t));
        if (udp) udp->chksum = 0;
    }
    return opt61_added;
}

static bool snoop_dhcp_reply(struct pbuf *p, uint16_t eth_ip_udp_hdr_len, uint8_t chaddr_out[6])
{
    dhcp_msg_t *dhcp = (dhcp_msg_t *)pkt_at(p, eth_ip_udp_hdr_len, sizeof(dhcp_msg_t));
    if (!dhcp || dhcp->op != DHCP_OP_REPLY || dhcp->hlen != 6 || dhcp->magic != htonl(DHCP_MAGIC_COOKIE)) return false;

    uint8_t orig_mac[6];
    if (xid_map_lookup(dhcp->xid, orig_mac)) {
        memcpy(chaddr_out, orig_mac, 6);
        memcpy(dhcp->chaddr, orig_mac, 6);
        udp_hdr_t *udp = (udp_hdr_t *)pkt_at(p, eth_ip_udp_hdr_len - (uint16_t)sizeof(udp_hdr_t), sizeof(udp_hdr_t));
        if (udp) udp->chksum = 0;
    } else {
        memcpy(chaddr_out, dhcp->chaddr, 6);
    }

    if (dhcp->yiaddr != 0) {
        uint16_t opts_len = p->len - eth_ip_udp_hdr_len - (uint16_t)sizeof(dhcp_msg_t);
        uint8_t optlen = 0;
        uint8_t *opt = dhcp_find_option(dhcp->options, opts_len, 53, &optlen);
        if (opt && optlen >= 1 && opt[0] == DHCP_MSG_ACK) {
            fdb_insert(dhcp->yiaddr, chaddr_out);
        }
    }
    return true;
}

/* -------------------------------------------------------------------------
 * Output & Input Hooks
 * ------------------------------------------------------------------------- */
static err_t bridge_output_sta(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    if (!s_bridge_enabled) return s_orig_output_sta(netif, p, ipaddr);

    uint8_t client_mac[6];
    if (fdb_lookup(ipaddr->addr, client_mac) && pbuf_header(p, sizeof(eth_hdr_t)) == 0) {
        eth_hdr_t *eth = (eth_hdr_t *)p->payload;
        memcpy(eth->dst, client_mac, 6);
        memcpy(eth->src, s_ap_nif->hwaddr, 6);
        eth->type = htons(ETHTYPE_IP);

        err_t err = s_orig_lo_ap(s_ap_nif, p);
        pbuf_header(p, -(s16_t)sizeof(eth_hdr_t));
        return err;
    }

    // Forward IP multicast (224.0.0.0/4) to AP side
    uint32_t hip = ntohl(ipaddr->addr);
    if ((hip >> 28) == 0xE && pbuf_header(p, sizeof(eth_hdr_t)) == 0) {
        eth_hdr_t *eth = (eth_hdr_t *)p->payload;
        eth->dst[0] = 0x01; eth->dst[1] = 0x00; eth->dst[2] = 0x5e;
        eth->dst[3] = (uint8_t)((hip >> 16) & 0x7f);
        eth->dst[4] = (uint8_t)((hip >>  8) & 0xff);
        eth->dst[5] = (uint8_t)( hip        & 0xff);
        memcpy(eth->src, s_ap_nif->hwaddr, 6);
        eth->type = htons(ETHTYPE_IP);
        s_orig_lo_ap(s_ap_nif, p);
        pbuf_header(p, -(s16_t)sizeof(eth_hdr_t));
    }

    return s_orig_output_sta(netif, p, ipaddr);
}

static err_t bridge_output_ap(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    if (!s_bridge_enabled) return s_orig_output_sta(s_sta_nif, p, ipaddr);

    uint8_t client_mac[6];
    if (fdb_lookup(ipaddr->addr, client_mac) && pbuf_header(p, sizeof(eth_hdr_t)) == 0) {
        eth_hdr_t *eth = (eth_hdr_t *)p->payload;
        memcpy(eth->dst, client_mac, 6);
        memcpy(eth->src, s_ap_nif->hwaddr, 6);
        eth->type = htons(ETHTYPE_IP);
        err_t err = s_orig_lo_ap(s_ap_nif, p);
        pbuf_header(p, -(s16_t)sizeof(eth_hdr_t));
        return err;
    }
    return s_orig_output_sta(s_sta_nif, p, ipaddr);
}

static err_t bridge_input_ap(struct pbuf *p, struct netif *inp)
{
    if (!s_bridge_enabled) return s_orig_input_ap(p, inp);
    if (p->len < sizeof(eth_hdr_t)) return s_orig_input_ap(p, inp);

    eth_hdr_t *eth_p = (eth_hdr_t *)p->payload;
    bool is_bcast = (eth_p->dst[0] & 0x01) != 0;
    bool is_to_ap_mac = (memcmp(eth_p->dst, s_ap_nif->hwaddr, 6) == 0);
    uint16_t eth_type = ntohs(eth_p->type);

    if (eth_type == ETHTYPE_ARP) {
        arp_hdr_t *arp = (arp_hdr_t *)pkt_at(p, sizeof(eth_hdr_t), sizeof(arp_hdr_t));
        if (arp) fdb_insert(arp->spa, eth_p->src);
    } else if (eth_type == ETHTYPE_IP) {
        ip_hdr_t *ip = (ip_hdr_t *)pkt_at(p, sizeof(eth_hdr_t), sizeof(ip_hdr_t));
        if (ip) fdb_insert(ip->src, eth_p->src);
    }

    if (is_to_ap_mac && !is_bcast) {
        return s_orig_input_ap(p, inp);
    }

    struct pbuf *q = pbuf_alloc(PBUF_RAW, p->tot_len + 16, PBUF_RAM);
    if (!q) return s_orig_input_ap(p, inp);
    pbuf_copy(q, p);

    eth_hdr_t *eth = (eth_hdr_t *)q->payload;
    bool handled = false;
    bool opt61_added = false;
    memcpy(eth->src, s_sta_nif->hwaddr, 6);

    uint32_t sta_ip = netif_ip4_addr(s_sta_nif)->addr;

    if (eth_type == ETHTYPE_ARP) {
        arp_hdr_t *arp = (arp_hdr_t *)pkt_at(q, sizeof(eth_hdr_t), sizeof(arp_hdr_t));
        if (arp) {
            fdb_insert(arp->spa, ((eth_hdr_t*)p->payload)->src);
            if (ntohs(arp->op) == 1 && sta_ip != 0 && arp->tpa == sta_ip) {
                send_proxy_arp_reply(s_ap_nif, s_orig_lo_ap, arp, sta_ip);
                handled = true;
            } else {
                memcpy(arp->sha, s_sta_nif->hwaddr, 6);
                pbuf_realloc(q, p->tot_len);
                s_orig_lo_sta(s_sta_nif, q);
                handled = true;
            }
        }
    } else if (eth_type == ETHTYPE_IP) {
        ip_hdr_t *ip = (ip_hdr_t *)pkt_at(q, sizeof(eth_hdr_t), sizeof(ip_hdr_t));
        if (ip) {
            fdb_insert(ip->src, ((eth_hdr_t*)p->payload)->src);
            if (ip->proto == 17) {
                uint16_t udp_off = sizeof(eth_hdr_t) + (ip->vhl & 0x0f) * 4;
                udp_hdr_t *udp = (udp_hdr_t *)pkt_at(q, udp_off, sizeof(udp_hdr_t));
                if (udp && ntohs(udp->dst_port) == 67) {
                    opt61_added = snoop_dhcp_request(q, udp_off + sizeof(udp_hdr_t));
                }
            }
            pbuf_realloc(q, opt61_added ? p->tot_len + 9 : p->tot_len);
            s_orig_lo_sta(s_sta_nif, q);
            handled = true;
        }
    } else {
        pbuf_realloc(q, p->tot_len);
        s_orig_lo_sta(s_sta_nif, q);
        handled = true;
    }

    pbuf_free(q);
    if (is_bcast) return s_orig_input_ap(p, inp);
    if (handled) {
        pbuf_free(p);
        return ERR_OK;
    }
    return s_orig_input_ap(p, inp);
}

static err_t bridge_input_sta(struct pbuf *p, struct netif *inp)
{
    if (!s_bridge_enabled) return s_orig_input_sta(p, inp);
    if (p->len < sizeof(eth_hdr_t)) return s_orig_input_sta(p, inp);

    eth_hdr_t *eth_p = (eth_hdr_t *)p->payload;
    if (memcmp(eth_p->src, s_sta_nif->hwaddr, 6) == 0 || memcmp(eth_p->src, s_ap_nif->hwaddr, 6) == 0) {
        return s_orig_input_sta(p, inp);
    }

    struct pbuf *q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
    if (!q) return s_orig_input_sta(p, inp);
    pbuf_copy(q, p);

    eth_hdr_t *eth = (eth_hdr_t *)q->payload;
    uint16_t eth_type = ntohs(eth->type);
    bool is_bcast = (eth->dst[0] & 0x01) != 0;
    bool is_to_sta_mac = (memcmp(eth->dst, s_sta_nif->hwaddr, 6) == 0);
    bool handled = false;
    memcpy(eth->src, s_ap_nif->hwaddr, 6);

    uint32_t sta_ip = netif_ip4_addr(s_sta_nif)->addr;

    if (eth_type == ETHTYPE_IP) {
        ip_hdr_t *ip = (ip_hdr_t *)pkt_at(q, sizeof(eth_hdr_t), sizeof(ip_hdr_t));
        if (ip) {
            uint8_t ch[6];
            bool have_dhcp = false;
            if (ip->proto == 17) {
                uint16_t udp_off = sizeof(eth_hdr_t) + (ip->vhl & 0x0f) * 4;
                udp_hdr_t *udp = (udp_hdr_t *)pkt_at(q, udp_off, sizeof(udp_hdr_t));
                if (udp && ntohs(udp->src_port) == 67 && ntohs(udp->dst_port) == 68) {
                    have_dhcp = snoop_dhcp_reply(q, udp_off + sizeof(udp_hdr_t), ch);
                }
            }
            uint8_t mac_buf[6];
            bool have_mac = false;
            if (have_dhcp) {
                memcpy(mac_buf, ch, 6);
                have_mac = true;
            } else if (!is_bcast) {
                if (sta_ip != 0 && ip->dst != sta_ip) {
                    have_mac = fdb_lookup(ip->dst, mac_buf);
                }
            }
            if (is_bcast || have_mac) {
                if (have_mac) memcpy(eth->dst, mac_buf, 6);
                s_orig_lo_ap(s_ap_nif, q);
                handled = true;
            }
        }
    } else if (eth_type == ETHTYPE_ARP) {
        arp_hdr_t *arp = (arp_hdr_t *)pkt_at(q, sizeof(eth_hdr_t), sizeof(arp_hdr_t));
        if (arp) {
            uint8_t proxy_mac[6];
            if (ntohs(arp->op) == 1 && fdb_lookup(arp->tpa, proxy_mac)) {
                send_proxy_arp_reply(s_sta_nif, s_orig_lo_sta, arp, arp->tpa);
                handled = true;
                pbuf_free(q);
                pbuf_free(p);
                return ERR_OK;
            }
            memcpy(arp->sha, s_ap_nif->hwaddr, 6);
            uint8_t target_mac[6];
            bool have_target_mac = false;
            if (!is_bcast) {
                if (sta_ip != 0 && arp->tpa != sta_ip) {
                    have_target_mac = fdb_lookup(arp->tpa, target_mac);
                }
            }
            if (is_bcast || have_target_mac) {
                if (have_target_mac) {
                    memcpy(eth->dst, target_mac, 6);
                    memcpy(arp->tha, target_mac, 6);
                }
                s_orig_lo_ap(s_ap_nif, q);
                handled = true;
            }
        }
    }

    pbuf_free(q);
    if (is_bcast || (is_to_sta_mac && !handled)) {
        return s_orig_input_sta(p, inp);
    }
    pbuf_free(p);
    return ERR_OK;
}

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */
void bridge_init(struct netif *sta_netif, struct netif *ap_netif)
{
    if (s_bridge_enabled) return;
    s_sta_nif = sta_netif;
    s_ap_nif  = ap_netif;

    portENTER_CRITICAL(&s_bridge_mux);
    memset(s_fdb, 0, sizeof(s_fdb));
    memset(s_xid_map, 0, sizeof(s_xid_map));
    portEXIT_CRITICAL(&s_bridge_mux);

    s_orig_input_sta  = sta_netif->input;
    s_orig_input_ap   = ap_netif->input;
    s_orig_lo_sta     = sta_netif->linkoutput;
    s_orig_lo_ap      = ap_netif->linkoutput;
    s_orig_output_sta = sta_netif->output;
    s_orig_output_ap  = ap_netif->output;

    sta_netif->input  = bridge_input_sta;
    ap_netif->input   = bridge_input_ap;
    sta_netif->output = bridge_output_sta;
    ap_netif->output  = bridge_output_ap;

    netif_set_default(sta_netif);

    s_bridge_enabled = true;
    ESP_LOGI(TAG, "Layer-2 transparent bridge initialized (STA <-> SoftAP)");
}

void bridge_deinit(void)
{
    if (!s_bridge_enabled) return;
    s_bridge_enabled = false;

    if (s_sta_nif && s_orig_input_sta) {
        s_sta_nif->input  = s_orig_input_sta;
        s_sta_nif->output = s_orig_output_sta;
    }
    if (s_ap_nif && s_orig_input_ap) {
        s_ap_nif->input  = s_orig_input_ap;
        s_ap_nif->output = s_orig_output_ap;
    }

    portENTER_CRITICAL(&s_bridge_mux);
    memset(s_fdb, 0, sizeof(s_fdb));
    memset(s_xid_map, 0, sizeof(s_xid_map));
    portEXIT_CRITICAL(&s_bridge_mux);

    ESP_LOGI(TAG, "Layer-2 bridge deinitialized");
}

bool bridge_is_active(void)
{
    return s_bridge_enabled;
}

void bridge_show_fdb(void)
{
    uint32_t now = now_secs();
    ESP_LOGI(TAG, "Bridge FDB (IP -> Client MAC):");

    fdb_entry_t snapshot[FDB_SIZE];
    int count = 0;

    portENTER_CRITICAL(&s_bridge_mux);
    for (int i = 0; i < FDB_SIZE; i++) {
        if (s_fdb[i].ip != 0 && s_fdb[i].expires_s > now) {
            snapshot[count++] = s_fdb[i];
        }
    }
    portEXIT_CRITICAL(&s_bridge_mux);

    for (int i = 0; i < count; i++) {
        esp_ip4_addr_t addr;
        addr.addr = snapshot[i].ip;
        ESP_LOGI(TAG, "  " IPSTR " -> %02x:%02x:%02x:%02x:%02x:%02x (expires in %d s)",
                 IP2STR(&addr),
                 snapshot[i].mac[0], snapshot[i].mac[1], snapshot[i].mac[2],
                 snapshot[i].mac[3], snapshot[i].mac[4], snapshot[i].mac[5],
                 (int)(snapshot[i].expires_s - now));
    }
    if (count == 0) {
        ESP_LOGI(TAG, "  (empty)");
    }
}
