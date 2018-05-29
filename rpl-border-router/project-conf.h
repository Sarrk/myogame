/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 */

#ifndef PROJECT_ROUTER_CONF_H_
#define PROJECT_ROUTER_CONF_H_

#ifndef UIP_FALLBACK_INTERFACE
#define UIP_FALLBACK_INTERFACE rpl_interface
#endif

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          32
#endif

#ifndef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    140
#endif

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60
#endif

#ifndef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2
#endif

#define CC2650_FAST_RADIO_STARTUP               1
#define WITH_FAST_SLEEP                         0

#define RPL_CONF_MOP RPL_MOP_NON_STORING

#define CONTIKIMAC_CONF_INTER_PACKET_INTERVAL (RTIMER_SECOND / 240)


//#define CSMA_CONF_MIN_BE 3
//#define CSMA_CONF_MAX_BE 5
// #if RTIMER_ARCH_SECOND > 8000
// #define CONTIKIMAC_CONF_CCA_SLEEP_TIME                    RTIMER_ARCH_SECOND / 1000
// #else
// #define CONTIKIMAC_CONF_CCA_SLEEP_TIME                     (RTIMER_ARCH_SECOND / 1000) + 1
// #endif 


//#define CONTIKIMAC_CONF_MAX_SILENCE_PERIODS 10
//#define CONTIKIMAC_CONF_SEND_SW_ACK               0
//#define IEEE_MODE_CONF_AUTOACK               0

 //#undef RPL_CONF_WITH_DAO_ACK
 //#define RPL_CONF_WITH_DAO_ACK          0

// #define UIP_CONF_ND6_SEND_NA              1
//#define UIP_CONF_ND6_SEND_RA                0
//#define UIP_CONF_ND6_SEND_NS                0
// #undef COAP_LINK_FORMAT_FILTERING
// #define COAP_LINK_FORMAT_FILTERING     0
// #undef COAP_PROXY_OPTION_PROCESSING
// #define COAP_PROXY_OPTION_PROCESSING   0

//#define NETSTACK_CONF_NETWORK sicslowpan_driver
//#define NETSTACK_CONF_MAC     nullmac_driver
//#define NETSTACK_CONF_RDC     nullrdc_driver
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 32

#endif /* PROJECT_ROUTER_CONF_H_ */
