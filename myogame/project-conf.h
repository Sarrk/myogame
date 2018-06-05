#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_








// #define NETSTACK_CONF_NETWORK sicslowpan_driver
 //#define NETSTACK_CONF_MAC     nullmac_driver
//#define NETSTACK_CONF_RDC     nullrdc_driver
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 32
#define CC2650_FAST_RADIO_STARTUP               1
//#define WITH_FAST_SLEEP                         0 // Gets clobbered by contiki-conf.h
//#define CONTIKIMAC_CONF_SEND_SW_ACK               0
//#define CONTIKIMAC_CONF_MAX_SILENCE_PERIODS 10

//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 128


#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          32
#endif

#define RPL_CONF_MOP RPL_MOP_NON_STORING

//#define IEEE_MODE_CONF_AUTOACK               0
// #define UIP_CONF_ND6_SEND_NA              1
// #define UIP_CONF_ND6_SEND_RA                0
// #define UIP_CONF_ND6_SEND_NS                0


 //#undef RPL_CONF_WITH_DAO_ACK
 //#define RPL_CONF_WITH_DAO_ACK          0
// #undef COAP_LINK_FORMAT_FILTERING
// #define COAP_LINK_FORMAT_FILTERING     0
// #undef COAP_PROXY_OPTION_PROCESSING
// #define COAP_PROXY_OPTION_PROCESSING   0

#endif // PROJECT_CONF_H_