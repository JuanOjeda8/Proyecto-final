/*
 *Proyecto
 *BY team juan ojeda
 */

/**
 * @file    project01.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define MAXCOUNT         15u
#define SEM_INIT         0u
#define PRIORIDAD        4u

/*Se define horario del reloj*/
#define HORA             8u
#define MINUTO	         59u
#define SEGUNDO	         40u

/*Defino horario de la alarma*/
#define ALARMA_HORA      8u
#define ALARMA_MINUTO	 31u
#define ALARMA_SEGUNDO	 00u

/*Prototipado de las tareas*/
void second_task(void *args);
void minute_task(void *args);
void hours_task(void *args);
void alarm_task(void *args);
void print_task(void *args);

/*Estructura de cola*/
typedef enum{second_type,minutes_type,hours_type} time_types_t;
typedef struct{
	time_types_t time_type;
	uint8_t value;
}time_msg_t;


typedef struct {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

}clocksystem;

/*Se inicializa el reloj del sistema*/
clocksystem reloj ={HORA,MINUTO,SEGUNDO};

/*Se fija la alarma*/
clocksystem alarm ={ALARMA_HORA,ALARMA_MINUTO,ALARMA_SEGUNDO};

/*Variable para el semaforo*/
SemaphoreHandle_t minutes_semaphore,hours_semaphore;
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();


#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Inicia\n");
    /*creacion de semaforos*/
    minutes_semaphore = xSemaphoreCreateBinary();
    hours_semaphore = xSemaphoreCreateBinary();

    /*creacion de tareas*/
    if((minutes_semaphore && hours_semaphore)!=0){
    xTaskCreate(second_task, "Second_Task", 250,(void*) &reloj, PRIORIDAD, NULL);
    xTaskCreate(minute_task, "Minute_Task", 250, (void*) &reloj, PRIORIDAD-1, NULL);
    xTaskCreate(hours_task, "Hours_Task", 250, (void*)&reloj, PRIORIDAD-2, NULL);
    xTaskCreate(alarm_task, "Alarm_Task", 250, NULL, PRIORIDAD-3, NULL);
    xTaskCreate(print_task, "Print_Task", 250, NULL, PRIORIDAD-3, NULL);
    vTaskStartScheduler();
    						}
    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}

void second_task(void *args){
	clocksystem *hora;
	hora=(clocksystem*)args;
	uint8_t second;
	second=hora->second;
	portTickType LastWakeTime;
	LastWakeTime = xTaskGetTickCount();
while(true){

	second++;
	PRINTF("SEGUNDERO  %d \n ",second );
	if(second==60){
		second=0;
		xSemaphoreGive(minutes_semaphore);
					}

	vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(1000));
		}
}
void minute_task(void *args){

	clocksystem *hora;
	hora=(clocksystem*)args;
	uint8_t minutes;
	minutes=hora->minute;

	while(true){

		minutes++;
		hora->minute=minutes;
		PRINTF("MINUTERO %d \n ",minutes);
		xSemaphoreTake(minutes_semaphore,portMAX_DELAY);

			if(minutes==60){
				minutes=0;
				xSemaphoreGive(hours_semaphore);
							}


				}

	}


void hours_task(void *args){

	clocksystem *hora;
	hora=(clocksystem*)args;
	uint8_t hours;
	hours=hora->hour;

	while(true){
			hours++;
			PRINTF("HORA %d \n ",hours);
			xSemaphoreTake(hours_semaphore,portMAX_DELAY);

			if(hours==60){
				hours=0;
				xSemaphoreTake(hours_semaphore, portMAX_DELAY );
						}

					}


}
void alarm_task(void *args){
	while(true){

		}


}
void print_task(void *args){
	while(true){

		}

}

