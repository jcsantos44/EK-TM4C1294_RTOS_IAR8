#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define BUFFER_SIZE 8

osThreadId_t produtor_id, consumidor_id;
osSemaphoreId_t vazio_id, cheio_id;
uint8_t buffer[BUFFER_SIZE];
uint8_t index_i = 0, count = 0;

void consumidor(void *arg){
  uint8_t index_o = 0, state;
  
  while(1){
    osSemaphoreAcquire(cheio_id, osWaitForever); // há dado disponível?
    state = buffer[index_o]; // retira do buffer
    osSemaphoreRelease(vazio_id); // sinaliza um espaço a mais
    
    index_o++;
    if(index_o >= BUFFER_SIZE) // incrementa índice de retirada do buffer
      index_o = 0;
    
    LEDWrite(LED4 | LED3 | LED2 | LED1, state); // apresenta informação consumida
    osDelay(10);
  } // while
} // consumidor

void Switch_handler_produtor(void)
{
    osSemaphoreAcquire(vazio_id, 0); // há espaço disponível?
    buffer[index_i] = count; // coloca no buffer
    osSemaphoreRelease(cheio_id); // sinaliza um espaço a menos
    
    index_i++; // incrementa índice de colocação no buffer
    if(index_i >= BUFFER_SIZE)
      index_i = 0;
    
    count++;
    count &= 0x0F; // produz nova informação
  
  GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0);
}

void main(void){
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // push-buttons SW1 e SW2 como entrada
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  // Habilita interrupção
  GPIOIntRegister(GPIO_PORTJ_BASE, Switch_handler_produtor);
  GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);
  GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0);
  
  SystemInit();
  LEDInit(LED4 | LED3 | LED2 | LED1);

  osKernelInitialize();

  consumidor_id = osThreadNew(consumidor, NULL, NULL);

  vazio_id = osSemaphoreNew(BUFFER_SIZE, BUFFER_SIZE, NULL); // espaços disponíveis = BUFFER_SIZE
  cheio_id = osSemaphoreNew(BUFFER_SIZE, 0, NULL); // espaços ocupados = 0
  
  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
} // main
