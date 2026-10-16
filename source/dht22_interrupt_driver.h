/*
 * dht22_driver.h
 *
 *  Created on: 6 maj 2026
 *      Author: silnymaciej
 */

#ifndef DHT22_INTERRUPT_DRIVER_H_
#define DHT22_INTERRUPT_DRIVER_H_

#include "pin_mux.h"
#include "board.h"
#include "delay_us.h"

#define DHT22_1_GPIO 					BOARD_INIT_DHT22_PINS_DHT22_GPIO
#define DHT22_1_PIN					    BOARD_INIT_DHT22_PINS_DHT22_GPIO_PIN


typedef struct{
	GPIO_Type* base;
	uint8_t pin;

	uint32_t delay_us;

	volatile bool measures_ready;

	uint8_t temp_integral;

	uint8_t temp_decimal;

	bool temp_negative;

	uint8_t rh_integral;

	uint8_t rh_decimal;

	bool trigger_did;

	bool trigger_delay;

	volatile bool is_measuring;

	volatile uint32_t last_edge;

	volatile uint8_t bit_idx;

	uint32_t trigger_start_time;

	volatile uint8_t handshake_step;

	volatile uint8_t bit_tab[5];

}DHT22_Sensor;

extern DHT22_Sensor dht22_1;

#define DHT22_SENSOR  (&dht22_1)


typedef enum{
	DHT22_DATA_NOT_READY = 666,
	DHT22_DATA_READY = 555,
	DHT22_GND_ERROR = 333,
	DHT22_VCC_ERROR = 444,
	DHT22_CHECK_SUM_ERROR = 777,
	DHT22_TIMEOUT_ERROR = 111
}g_Sensor_DHT22_Check_t;


g_Sensor_DHT22_Check_t DHT22_Get_Temperature_And_RH(DHT22_Sensor* dht22);
void DHT22_Init(void);
#endif /* DHT22_INTERRUPT_DRIVER_H_ */
