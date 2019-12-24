#ifndef PLATFORM_H
#define PLATFORM_H

#include <plic.h>
#include <sifive_uart.h>
#include <const.h>

#define _REG64(p, i) (*(volatile uint64_t *)((p) + (i)))
#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))
#define _REG16(p, i) (*(volatile uint16_t *)((p) + (i)))

#define UART_CTRL_ADDR _AC(0x64000000,UL)
#define UART_CTRL_SIZE _AC(0x1000,UL)
#define UART_INT_BASE 1
#define UART_REG(offset) _REG32(UART_CTRL_ADDR, offset)
#define UART_REG64(offset) _REG64(UART_CTRL_ADDR, offset)

#define PLIC_CTRL_ADDR _AC(0xc000000,UL)
#define PLIC_CTRL_SIZE _AC(0x4000000,UL)
#define PLIC_REG(offset) _REG32(PLIC_CTRL_ADDR, offset)
#define PLIC_REG64(offset) _REG64(PLIC_CTRL_ADDR, offset)

#define REG32(p, i)	((p)[(i) >> 2])
//
//#ifndef UART_CTRL_ADDR
//  #ifndef UART_NUM
//    #define UART_NUM 0
//  #endif
//
//  #define _CONCAT3(A, B, C) A ## B ## C
//  #define _UART_CTRL_ADDR(UART_NUM) _CONCAT3(UART, UART_NUM, _CTRL_ADDR)
//  #define UART_CTRL_ADDR _UART_CTRL_ADDR(UART_NUM)
//#endif
static volatile uint32_t * const uart = (void *)(UART_CTRL_ADDR);


#endif /* PLATFORM_H */
