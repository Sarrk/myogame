/**
 * \file
 *         Prac 8
 *         
 * \author
 *         Michael Redpath
 */


/*
Quick side note. If that code doesn't work. 

Change This:
ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO6);
To This:
ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO7);
Also, you may need to push the push button to get the adc to start working (only needed to do this after I shorted my tag and changed the lines above)
*/


#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "random.h"
#include "board-peripherals.h"
#include "buzzer.h"
#include "dev/serial-line.h"
#include "dev/cc26xx-uart.h"
// #include "proj_adc.h"

#include "ti-lib.h"

#include <stdio.h>
#include <stdint.h>


static struct etimer et;


PROCESS(main_process, "main process");
PROCESS(adc_process, "adc process");
AUTOSTART_PROCESSES(&main_process, &adc_process);

int main_loop_timer = CLOCK_SECOND * 2;

/*******Func Declarations**********************/
int get_adc1();
int get_adc2();
void init_adc(int samples_per_second);
/*****************************/


PROCESS_THREAD(main_process, ev, data) {
	
  	PROCESS_BEGIN();

  	etimer_set(&et, main_loop_timer);	//Set event timer for 20s interval.
	
	init_adc(4);

  	while(1) {

    	PROCESS_YIELD();

		//Flash Green LED every 2s.
		if(ev == PROCESS_EVENT_TIMER) {
			if(data == &et) {
				leds_toggle(LEDS_GREEN);
				int adc_val = get_adc1();
				if(adc_val > 900) {
					printf("  !Muscle Movement!  -  ADC: %d\n\r\n\r", adc_val);
				} else {
					printf("No Muscle Movement   -  ADC: %d\n\r\n\r", adc_val);
				}
				etimer_set(&et, main_loop_timer);		//Reset event timer
			}
		}
	}
  PROCESS_END();
}

/*********************************************/
int adc1,adc2;
static struct etimer et_adc;
int timer_delay;

int get_adc1() {
    return adc1;
}

int get_adc2() {
    return adc2;
}

void init_adc(int samples_per_second) {
    timer_delay = CLOCK_SECOND / samples_per_second;
    etimer_set(&et_adc, timer_delay);
}

PROCESS_THREAD(adc_process, ev, data)
{
  PROCESS_BEGIN();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_adc));
    // printf("ENTERED LOOP\n\r");
    /* intialisation of ADC */
    ti_lib_aon_wuc_aux_wakeup_event(AONWUC_AUX_WAKEUP);
    while(!(ti_lib_aon_wuc_power_status_get() & AONWUC_AUX_POWER_ON));

    /*
     * Enable clock for ADC digital and analog interface (not currently enabled
     * in driver)
     */
    ti_lib_aux_wuc_clock_enable(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
                                AUX_WUC_SMPH_CLOCK);
    while(ti_lib_aux_wuc_clock_status(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
                                      AUX_WUC_SMPH_CLOCK)
          != AUX_WUC_CLOCK_READY);

    /* Connect AUX IO7 (DIO23, but also DP2 on XDS110) as analog input. */
    ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO6);

    /* Set up ADC range, AUXADC_REF_FIXED = nominally 4.3 V */
    ti_lib_aux_adc_enable_sync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US,
                               AUXADC_TRIGGER_MANUAL);

    /* Trigger ADC converting */
    ti_lib_aux_adc_gen_manual_trigger();

    /* Read value */
    adc1 = ti_lib_aux_adc_read_fifo();

    /* Shut the adc down */
    ti_lib_aux_adc_disable();

    etimer_reset(&et_adc);
  }

  PROCESS_END();
}

