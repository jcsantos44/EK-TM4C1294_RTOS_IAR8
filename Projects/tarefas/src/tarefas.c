#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS

osThreadId_t thread1_id, thread2_id, thread3_id, thread4_id;

struct Led
{
  uint8_t id;
  uint16_t delay;
} led1, led2, led3, led4;

//void thread1(void *arg){
//  uint8_t state = 0;
//  
//  while(1){
//    state ^= LED1;
//    LEDWrite(LED1, state);
//    osDelay(100);
//  } // while
//} // thread1

void thread2(void *arg){
  uint8_t state = 0;
  uint32_t tick;
  struct Led led = *(struct Led*) arg;
  
  while(1){
    tick = osKernelGetTickCount();
    
    state ^= led.id;
    LEDWrite(led.id, state);
    
    osDelayUntil(tick + led.delay);
  } // while
} // thread2

void main(void){
  led1.id = LED1;
  led2.id = LED2;
  led3.id = LED3;
  led4.id = LED4;
  
  led1.delay = 200;
  led2.delay = 300;
  led3.delay = 500;
  led4.delay = 700;
  
  LEDInit(LED2 | LED1);

  osKernelInitialize();

  thread1_id = osThreadNew(thread2, &led1, NULL);
  thread2_id = osThreadNew(thread2, &led2, NULL);
  thread3_id = osThreadNew(thread2, &led3, NULL);
  thread4_id = osThreadNew(thread2, &led4, NULL);

  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
} // main
