/*
 * This file contains all the code required to connect to the rpl router via udp
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "net/rpl/rpl.h"


#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *client_conn;
uip_ipaddr_t rpl_ipaddr;

/*
 * @brief  Send data to python game via udp
 * 
 * @param  char *data : data to send
 *
 * @return None
 */
void udp_send(char *data) {
  
    PRINTF("Client sending to RPL: ");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF(" (msg: %s)\n\r", data);
  #if SEND_TOO_LARGE_PACKET_TO_TEST_FRAGMENTATION
    uip_udp_packet_send(client_conn, data, UIP_APPDATA_SIZE);
  #else /* SEND_TOO_LARGE_PACKET_TO_TEST_FRAGMENTATION */
    uip_udp_packet_send(client_conn, data, strlen(data));
  #endif /* SEND_TOO_LARGE_PACKET_TO_TEST_FRAGMENTATION */

}

/*
 * @brief  Print the device address.
 *
 * @param  None
 *
 * @return None
 */
static void print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n\r");
    }
  }
}

/*
 * @brief  Connect to rpl router
 * 
 * @param  None
 * 
 * @return None
 */
void init_udp(void) {
    PRINTF("Initialising UDP Connection\n\r");
    print_local_addresses();

    uip_ip6addr(&rpl_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x1);

    //Create UDP socket that listens for packets to PORT:3000 ***from*** PORT:3001
    client_conn = udp_new(&rpl_ipaddr, UIP_HTONS(3001), NULL);
    udp_bind(client_conn, UIP_HTONS(3000));

    PRINTF("Created a connection with the RPL ");
    PRINT6ADDR(&client_conn->ripaddr);
    PRINTF(" local/remote port %u/%u\n\r",
    UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

}   