/*
 * This is the mainfile of the mygame project
 * 
 * Michael Redpath  43237166
 * Michael Garthe   43198856
 * Callum Sinclair  42337612
 * Athon Milane     XXXXXXXX
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"

#include "lib/sensors.h"
#include "mpu-9250-sensor.h"
#include "ti-lib.h"
#include "board-peripherals.h"

#include <stdio.h>
#include <string.h>

#include "adc.h"
#include "udpclient.h"

#define DEBUG DEBUG_PRINT

#define SAMPLE_FREQ 1
#define SAMPLE_INTERVAL CLOCK_SECOND / SAMPLE_FREQ

PROCESS(sensor_process, "Sensor process");
PROCESS(udp_process, "UDP process");
AUTOSTART_PROCESSES(&sensor_process, &udp_process);         //sensor process will be started after calibration

int cnt = 0;


typedef struct myoData {
    int adc;
    int accX;
    int accY;
    int accZ;
    int gyroX;
    int gyroY;
    int gyroZ;
}  myoData;

uint8_t mode = 0x07;        //will be changed in calibration

myoData sensorData;



/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensor_process, ev, data) {
    static struct etimer adc_et;
    PROCESS_BEGIN();
    
    if (mode & 0x01) {
        init_adc();
    }
    if (mode & 0x06) {
        mpu_9250_sensor.configure(SENSORS_HW_INIT, 1);
    }

    etimer_set(&adc_et, SAMPLE_INTERVAL);
    while(1) {
        PROCESS_YIELD();
        if(etimer_expired(&adc_et)) {
            if (mode & 0x01) {
                sensorData.adc = sample_adc();
            }
            if (mode & 0x02) {
                sensorData.accX = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_X);
                sensorData.accY = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Y);
                sensorData.accZ = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Z);
            }
            if (mode & 0x04) {
                sensorData.gyroX = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_X);
                sensorData.gyroY = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Y);
                sensorData.gyroZ = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Z);
            }
            process_post_synch(&udp_process, PROCESS_EVENT_CONTINUE, &sensorData);
            if ((mode  & 0x06) == 6) {
                mpu_9250_sensor.configure(SENSORS_ACTIVE, MPU_9250_SENSOR_TYPE_ALL);
            } else if (mode  & 0x02) {
                mpu_9250_sensor.configure(SENSORS_ACTIVE, MPU_9250_SENSOR_TYPE_ACC_ALL);
            } else if (mode & 0x04) {
                mpu_9250_sensor.configure(SENSORS_ACTIVE, MPU_9250_SENSOR_TYPE_GYRO_ALL);
            }
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
         PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        if(data != NULL) {
            sprintf(msg, "Received message %03d - %d", cnt++, sensorData.adc);
            udp_send(msg);
        }
     }

     PROCESS_END();
 }



/*---------------------------------------------------------------------------*/
