/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
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

static struct uip_udp_conn *server_conn;

//unsigned int UTC = 0;
//unsigned int recTime = 0;
//static struct etimer let;
//int rssi = 0;
uint8_t calibrated = 0;
uint8_t connection_est = 0;
//char send_buf[2];
unsigned long seq = 0;

static struct rtimer timer_rtimer;		//Realtime Timer (for contiki scheduler)
static rtimer_clock_t timeout_rtimer = RTIMER_SECOND / 50;

PROCESS(udp_server_process, "UDP server process");
//PROCESS(LED_flasher, "LED flashing process");
AUTOSTART_PROCESSES(&resolv_process, &udp_server_process);


/*---------------------------------------------------------------------------*/
static void tcpip_handler(void) {
  char *str;

  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';
    seq = 0;
    PRINTF("REVIECED PACKEt\n");
    //UTC = str[0] + str[1] * 256 + str[2] * 256 * 256 + str[3] * 256 * 256 * 256;
    //recTime = clock_seconds();
    //rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
    if (!connection_est) {
      //  printf("Server IP %s", &server_conn->ripaddr);
      PRINTF("Connection Est\n");
        uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
        connection_est = 1;
    }
    
  }
}

/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
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

static void timeout_handler(void) {
  char buf[MAX_PAYLOAD_LEN];
 // char buf[2];

  sprintf(buf, "%lu", seq);
  seq++;
 // buf[0] = 'a';
 // buf[1] = '\0';
  uip_udp_packet_send(server_conn, buf, strlen(buf));
  leds_toggle(LEDS_GREEN);

  //rearm timer
  rtimer_set(&timer_rtimer, RTIMER_NOW() + timeout_rtimer, 0, timeout_handler, NULL);
}

static void calibration_check() {
    if (connection_est) {
        calibrated = 1;
        leds_off(LEDS_RED);
        rtimer_set(&timer_rtimer, RTIMER_NOW() + timeout_rtimer, 0, timeout_handler, NULL);
    } else {
        calibrated = 0;
    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data) {
#if UIP_CONF_ROUTER
  uip_ipaddr_t ipaddr;
#endif /* UIP_CONF_ROUTER */

    PROCESS_BEGIN();
    PRINTF("UDP server started\n\r");

    //set rpl to feather mode
   // rpl_set_mode(RPL_MODE_FEATHER);

#if RESOLV_CONF_SUPPORTS_MDNS
    resolv_set_hostname("contiki-udp-server");
#endif

#if UIP_CONF_ROUTER
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_ROUTER */

    print_local_addresses();

    //Create UDP socket and bind to port 3000
    server_conn = udp_new(NULL, UIP_HTONS(3001), NULL);
    udp_bind(server_conn, UIP_HTONS(3000));

   // send_buf[0] = 'a';
    //send_buf[1] = '\0';

    //char buf[MAX_PAYLOAD_LEN];
    //static struct etimer et;
  //  etimer_set(&et, SEND_INTERVAL);
    //etimer_set(&let, 0);
    leds_on(LEDS_RED);
    while(1) {
        if (!calibrated) {
            while(!calibrated) {
                PROCESS_YIELD();
                if (ev == tcpip_event) {
                    tcpip_handler();
                    calibration_check();
                }
            }
        } else {
            PROCESS_YIELD();     //might need a yield so the UIP process can run...?
           //timeout_handler();
          // uip_udp_packet_send(server_conn, send_buf, strlen(send_buf));
           //clock_wait(CLOCK_SECOND/10);
           
            
           
            // if (etimer_expired(&et)) {
            //     timeout_handler();
            //     etimer_restart(&et);
            // }
        }
    }
    PROCESS_END();
}



/*---------------------------------------------------------------------------*/
