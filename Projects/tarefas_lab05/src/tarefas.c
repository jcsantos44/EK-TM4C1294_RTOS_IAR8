#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS

osThreadId_t thread1_id, thread2_id;

osMutexId_t mutex1_id;

struct Led
{
  uint8_t id;
  uint16_t delay;
} led1, led2;

void thread1(void *arg){
  uint8_t state = 0;
  uint32_t tick;
  struct Led led = *(struct Led*) arg;
  
  while(1){
    osMutexAcquire(mutex1_id, osWaitForever);
    tick = osKernelGetTickCount();
    
    state ^= led.id;
    LEDWrite(led.id, state);
    osDelayUntil(tick + led.delay);
    osMutexRelease(mutex1_id);
  } // while
} // thread1

void thread2(void *arg){
  uint8_t state = 0;
  uint32_t tick;
  struct Led led = *(struct Led*) arg;
  
  while(1){
    osMutexAcquire(mutex1_id, osWaitForever);
    tick = osKernelGetTickCount();
    
    state ^= led.id;
    LEDWrite(led.id, state);
    osDelayUntil(tick + led.delay);
    osMutexRelease(mutex1_id);
  } // while
} // thread2

void main(void){
  led1.id = LED1;
  led2.id = LED2;
  
  led1.delay = 100;
  led2.delay = 100;
  
  LEDInit(LED2 | LED1);

  osKernelInitialize();
  
  mutex1_id = osMutexNew(NULL);

  thread1_id = osThreadNew(thread1, &led1, NULL);
  thread2_id = osThreadNew(thread2, &led2, NULL);

  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
} // main
