
#include "dht22_interrupt_driver.h"
#include "pin_mux.h"
#include <string.h>
#include "fsl_debug_console.h"

#define DHT22_MEASURE_DELAY		  2000000U
#define DHT22_START_LOW_US        2000U
#define DHT22_RELEASE_WAIT_US     30U
#define DHT22_TIMEOUT_US          10000U
#define DHT22_BIT_THRESHOLD_US    100U
#define DHT22_BITS                40U


DHT22_Sensor dht22_1 = {
	.base = DHT22_1_GPIO,
	.pin  = DHT22_1_PIN,
};

void DHT22_Interrupt(DHT22_Sensor* dht22){
	uint32_t now = CTIMER_GetTimerCountValue(CTIMER0);
	GPIO_PinClearInterruptFlag(dht22->base, dht22->pin);

	if(!dht22->is_measuring || dht22->measures_ready) return;

	uint32_t dt = now - dht22->last_edge;
	dht22->last_edge = now;


	if(dht22->handshake_step < 2U){
		++dht22->handshake_step;
		return;
	}


	if(dt > DHT22_BIT_THRESHOLD_US){
		dht22->bit_tab[dht22->bit_idx >> 3] |= (uint8_t)(0x80U >> (dht22->bit_idx & 7U));
	}

	if(++dht22->bit_idx == DHT22_BITS){
		dht22->is_measuring = false;
		dht22->measures_ready = true;
	}
}


static void DHT22_Reset_Flags(DHT22_Sensor* dht22){
	dht22->is_measuring = false;
	dht22->measures_ready = false;
	dht22->trigger_did = false;
	dht22->trigger_delay = false;
	dht22->last_edge = 0;
	dht22->trigger_start_time = 0;
	dht22->handshake_step = 0;
	dht22->bit_idx = 0;
	dht22->temp_negative = false;

	memset((void*)dht22->bit_tab, 0, 5);
}


static void DHT22_Do_Trigger(DHT22_Sensor* dht22){

	if(!dht22->trigger_delay){
		gpio_pin_config_t out_low = {kGPIO_DigitalOutput, 0U};
		GPIO_PinInit(dht22->base, dht22->pin, &out_low);

		dht22->trigger_start_time = CTIMER_GetTimerCountValue(CTIMER0);
		dht22->trigger_delay = true;
		return;
	}


	if((CTIMER_GetTimerCountValue(CTIMER0) - dht22->trigger_start_time) < DHT22_START_LOW_US) return;


	gpio_pin_config_t in = {kGPIO_DigitalInput, 0U};
	GPIO_PinInit(dht22->base, dht22->pin, &in);


	uint32_t t0 = CTIMER_GetTimerCountValue(CTIMER0);

	while(!GPIO_PinRead(dht22->base, dht22->pin) &&
	      (CTIMER_GetTimerCountValue(CTIMER0) - t0) < DHT22_RELEASE_WAIT_US){

	}
	delay_us(2U);
	GPIO_PinClearInterruptFlag(dht22->base, dht22->pin);

	dht22->trigger_start_time = CTIMER_GetTimerCountValue(CTIMER0);
	dht22->trigger_delay = false;
	dht22->trigger_did = true;
	dht22->is_measuring = true;
}


static void DHT22_Process_Sensor_Data(DHT22_Sensor* dht22, uint16_t raw_rh, uint16_t raw_temp){
	dht22->rh_integral = raw_rh / 10U;
	dht22->rh_decimal  = raw_rh % 10U;


	uint16_t t = raw_temp & 0x7FFFU;

	dht22->temp_negative = (raw_temp & 0x8000U) != 0U;
	dht22->temp_integral = t / 10U;
	dht22->temp_decimal  = t % 10U;
}


g_Sensor_DHT22_Check_t DHT22_Get_Temperature_And_RH(DHT22_Sensor* dht22){
	if(CTIMER_GetTimerCountValue(CTIMER0) - dht22->delay_us < DHT22_MEASURE_DELAY) return DHT22_DATA_NOT_READY;

	if(!dht22->trigger_did){
		DHT22_Do_Trigger(dht22);
		return DHT22_DATA_NOT_READY;
	}

	if(!dht22->measures_ready){
		if((CTIMER_GetTimerCountValue(CTIMER0) - dht22->trigger_start_time) >= DHT22_TIMEOUT_US){
			DHT22_Reset_Flags(dht22);
			return DHT22_TIMEOUT_ERROR;
		}
		return DHT22_DATA_NOT_READY;
	}


	uint8_t b[5];
	memcpy(b, (const void*)dht22->bit_tab, 5);
	DHT22_Reset_Flags(dht22);

	if((uint8_t)(b[0] + b[1] + b[2] + b[3]) != b[4]) return DHT22_CHECK_SUM_ERROR;

	uint16_t raw_rh   = (uint16_t)((b[0] << 8) | b[1]);
	uint16_t raw_temp = (uint16_t)((b[2] << 8) | b[3]);

	if(raw_rh > 1000U || (raw_temp & 0x7FFFU) > 1000U) return DHT22_CHECK_SUM_ERROR;

	DHT22_Process_Sensor_Data(dht22, raw_rh, raw_temp);

	dht22->delay_us = CTIMER_GetTimerCountValue(CTIMER0);
	return DHT22_DATA_READY;
}

void DHT22_Init(void){
	NVIC_ClearPendingIRQ(INT_0_IRQN);
	dht22_1.delay_us = CTIMER_GetTimerCountValue(CTIMER0);
	EnableIRQ(INT_0_IRQN);
}


void INT_0_IRQHANDLER(void){
	DHT22_Interrupt(&dht22_1);
	SDK_ISR_EXIT_BARRIER;
}

