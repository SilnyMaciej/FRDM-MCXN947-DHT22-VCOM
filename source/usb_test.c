/*
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    usb_test.c
 * @brief   Application entry point.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "usb_device_interface_0_cic_vcom.h"
#include "dht22_interrupt_driver.h"

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 *
 */




int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

     DHT22_Init();

    PRINTF("Hello World\r\n");

    g_Sensor_DHT22_Check_t dht22_status;
    usb_status_t status;

    while(1) {
    	USB_DeviceTasks();
    	dht22_status = DHT22_Get_Temperature_And_RH(DHT22_SENSOR);

    	if(dht22_status == DHT22_DATA_READY){

    		status = VCOM_SendTemperatureTask(DHT22_SENSOR->temp_integral, DHT22_SENSOR->temp_decimal, DHT22_SENSOR->rh_integral, DHT22_SENSOR->rh_decimal);

    		if(status == kStatus_USB_Success) PRINTF("%d.%d    %d.%d \r",DHT22_SENSOR->temp_integral, DHT22_SENSOR->temp_decimal, DHT22_SENSOR->rh_integral, DHT22_SENSOR->rh_decimal);

    		else PRINTF("Error %d        \r",(uint8_t)status);
    	}
    }
    return 0 ;
}


