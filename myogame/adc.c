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

#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip-debug.h"

#include "ti-lib.h"

#include <stdio.h>
#include <stdint.h>


/*
 * @brief  Sample the ADC.
 * 
 * @param  None
 *
 * @return int : ADC value.
 */
int sample_adc(void) {
    int adc1;

    /* intialisation of ADC */
    ti_lib_aon_wuc_aux_wakeup_event(AONWUC_AUX_WAKEUP);
    while(!(ti_lib_aon_wuc_power_status_get() & AONWUC_AUX_POWER_ON));

    ti_lib_aux_wuc_clock_enable(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
                                AUX_WUC_SMPH_CLOCK);
    while(ti_lib_aux_wuc_clock_status(AUX_WUC_ADI_CLOCK | AUX_WUC_ANAIF_CLOCK |
                                      AUX_WUC_SMPH_CLOCK)
          != AUX_WUC_CLOCK_READY);

    /* Connect AUX IO7 (DIO23, but also DP2 on XDS110) as analog input. */
    ti_lib_aux_adc_select_input(ADC_COMPB_IN_AUXIO7);

    /* Set up ADC range, AUXADC_REF_FIXED = nominally 4.3 V */
    ti_lib_aux_adc_enable_sync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_2P7_US,
                               AUXADC_TRIGGER_MANUAL);

    /* Trigger ADC converting */
    ti_lib_aux_adc_gen_manual_trigger();

    /* Read value */
    adc1 = ti_lib_aux_adc_read_fifo();

    /* Shut the adc down */
    ti_lib_aux_adc_disable();
    PRINTF("ADC > %05d \n\r", adc1);
    return adc1;
}

/*
 * @brief  Initialise the Grove ADC port to sample the MyoWare Muscle Sensor.
 * 
 * @param  None
 * 
 * @return None
 */
void init_adc(void) {
    PRINTF("ADC Initialised\n\r");
}   