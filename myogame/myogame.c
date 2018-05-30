/*
 * This is the mainfile of the mygame project
 * 
 * Michael Redpath  43237166
 * Michael Garthe   XXXXXXXX
 * Callum Sinclair  XXXXXXXX
 * Athon Milane     XXXXXXXX
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"

#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "udpclient.h"

#define DEBUG DEBUG_PRINT

#define SAMPLE_FREQ 50
#define SAMPLE_INTERVAL CLOCK_SECOND / SAMPLE_FREQ

PROCESS(adc_process, "ADC process");
PROCESS(udp_process, "UDP process");
AUTOSTART_PROCESSES(&adc_process, &udp_process);

int cnt = 0;


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(adc_process, ev, data) {
    static struct etimer adc_et;
    PROCESS_BEGIN();
    
    init_adc();

    etimer_set(&adc_et, SAMPLE_INTERVAL);
    while(1) {
        PROCESS_YIELD();
        if(etimer_expired(&adc_et)) {
            int adc_val = sample_adc();
            process_post_synch(&udp_process, PROCESS_EVENT_CONTINUE, &adc_val);
            etimer_restart(&adc_et);
        }
    }

    PROCESS_END();
}


/*
 * @brief  This process receives data from other processes and then sends that data over udp.
 *
 * @param  data : This parameter holds the data.
 *
 * @return None
 */
PROCESS_THREAD(udp_process, ev, data) {
    
    char msg[50];
    
    PROCESS_BEGIN();
    
    init_udp();
    while(1) {
        PROCESS_WAIT_EVENT();
        if(data != NULL) {
            sprintf(msg, "Received message %03d - %d", cnt++, *((int*) data));
            udp_send(msg);
        }
    }

    PROCESS_END();
}



/*---------------------------------------------------------------------------*/
