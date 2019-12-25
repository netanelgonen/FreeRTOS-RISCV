/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* RISCV includes */
#include "arch/syscalls.h"
#include "arch/clib.h"

extern uint64_t* ipc_mutex;

int main( void )
{
        //size_t free_heap_start = xPortGetFreeHeapSize();
				printf("ipc_mutex = %p\r\n",ipc_mutex);
        REG32(uart, UART_REG_TXCTRL) = UART_TXEN;
				for (int i;;++i)
        	printf("Booting Minimal Spike FreeRtos... Prepare tasks\r\n");
        return 0;
}
