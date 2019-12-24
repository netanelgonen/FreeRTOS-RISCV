/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* RISCV includes */
#include "arch/syscalls.h"
#include "arch/clib.h"



#define TASK_NUMBER (100)
#define NUM_OF_CMD (TASK_NUMBER*2)

#define QUEUE_LENGTH (5)

void mng( void *pvParameters );
void worker( void *pvParameters );

/* The queue that is added to the set. */
static QueueHandle_t xQueueArry[TASK_NUMBER];

/* The handle of the queue set to which the queue is added. */
static QueueSetHandle_t xQueueSet = NULL;

/* Set to pdFAIL if an error is detected by any queue set task.
ulCycleCounter will only be incremented if xQueueSetTasksSatus equals pdPASS. */
static volatile BaseType_t xQueueSetPollStatus = pdPASS;
static uint32_t cmd_completed = 0 ;
static TaskHandle_t  tasksArray[TASK_NUMBER+1];
static SemaphoreHandle_t cmdCompletedMutex;

#define suppres_print
#undef suppres_print

static inline void clear_mcycle(void) {
	__asm volatile ("csrw mcycle, zero");
}

static inline uint64_t read_mcycle(void) {
	uint64_t r = -1;
	__asm volatile ("csrr %0, mcycle":"+r"(r));
	return r;
}

int main( void )
{
        //size_t free_heap_start = xPortGetFreeHeapSize();
        REG32(uart, UART_REG_TXCTRL) = UART_TXEN;
        printf("Booting Minimal Spike FreeRtos... Prepare tasks\r\n");
        uint32_t i;
        uint64_t cycles = -1;
        for (i = 0; i < TASK_NUMBER; ++i)
        {
        	printf("Booting Minimal Spike FreeRtos... Prepare task %d configMAX_PRIORITIES: %d p: %d\r\n", i, configMAX_PRIORITIES, i % configMAX_PRIORITIES);
            xTaskCreate( worker, "worker", configMINIMAL_STACK_SIZE, (void*)i , i % configMAX_PRIORITIES, &(tasksArray[i]) );
            printf("Booting Minimal Spike FreeRtos... Prepare Qeue %d\r\n", i);
            xQueueArry[i] = xQueueCreate( QUEUE_LENGTH, sizeof( uint32_t ));
            printf("Booting Minimal Spike FreeRtos... Prepared %d\r\n", i);
        }
        
        xTaskCreate( mng, "mng", configMINIMAL_STACK_SIZE, NULL , (configMAX_PRIORITIES / 2), &(tasksArray[TASK_NUMBER]) );

        cmdCompletedMutex = xSemaphoreCreateMutex();

        /* Start the kernel.  From here on, only tasks and interrupts will run. */
        printf("Starting Scheduler...\r\n");
        clear_mcycle();
        vTaskStartScheduler();
        cycles = read_mcycle();
        printf("Finished, took %d cycles\r\n", cycles);
        /* Exit FreeRTOS */
        return 0;
}




void mng( void *pvParameters )
{
#ifndef suppres_print
	taskENTER_CRITICAL();
	printf("mng start\r\n");
	taskEXIT_CRITICAL();
#endif /* suppres_print */

    for (uint32_t i = 0; i < NUM_OF_CMD; ++i)
    {
#ifndef suppres_print
    	printf("Send to Q: %d\r\n", (i % TASK_NUMBER));
#endif
        xQueueSend( xQueueArry[(i % TASK_NUMBER)], ( void * )&i, portMAX_DELAY  );
    }
    while (cmd_completed != NUM_OF_CMD)
    {
        vTaskDelay(500);
    }
#ifndef suppres_print
    taskENTER_CRITICAL();
    printf("KILL THEM ALL\r\n");
    taskEXIT_CRITICAL();
#endif /* suppres_print */
    for ( uint32_t i = 0; TASK_NUMBER > i; ++i)
    {
    	vTaskDelete(tasksArray[i]);
    }
    vTaskEndScheduler();
}


void worker( void *pvParameters )
{
#ifndef suppres_print
	taskENTER_CRITICAL();
	printf("worker start %d\r\n", ((uint32_t)pvParameters));
	taskEXIT_CRITICAL();
#endif /* suppres_print */
    uint32_t cmd;
    while(1)
    {
    	xQueueReceive(xQueueArry[((uint32_t)pvParameters)], (void*)( &cmd ), portMAX_DELAY );
        xSemaphoreTake( cmdCompletedMutex, portMAX_DELAY );
        ++cmd_completed;
        xSemaphoreGive( cmdCompletedMutex );
    }
#ifndef suppres_print
    taskENTER_CRITICAL();
   	printf("worker end %d\r\n", (uint32_t)pvParameters);
    taskEXIT_CRITICAL();
#endif /* suppres_print */
}
