#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include "ns16550.h"

#ifndef __linux__

int read(int file, char *ptr, int len) {
  int todo;
  if(len == 0)
    return 0;
#ifdef CONSOLE_UART
  todo = 0;
  while (ns16550_rxready() && (todo < len)) {
    *ptr++ = ns16550_rxchar();
    todo++;
  }
#endif
  return todo;
}

int write(int file, char *ptr, int len) {
  int todo;

#ifdef CONSOLE_UART
  for (todo = 0; todo < len; todo++) {
    ns16550_txchar (*ptr++);
  }
#endif
  return len;
}

#define CLOCK_PERIOD  (30000000)
//#define CLOCK_PERIOD  (100) // simulation

// The following uses rdcycle instead of rdtime (which would make more sense),
// because rdtime causes an "illegal instruction" trap, which we do not yet
// handle in bare metal.

int gettimeofday(struct timeval *ptimeval, void *ptimezone)
{
    if (ptimeval)
    {
	long long tv;
#if __riscv_xlen == 64
	asm ("rdcycle %0" : "=r" (tv));
#else
	unsigned int tvh;
	unsigned int tvl;
	asm ("rdcycle %0;"
	    "rdcycleh %1 " : "=r" (tvl), "=r" (tvh));
	tv = ((long long)tvh) << 32 | tvl;
#endif
	ptimeval->tv_sec = tv / CLOCK_PERIOD;
	ptimeval->tv_usec = tv % CLOCK_PERIOD / (CLOCK_PERIOD / 1000000);
    }

    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    seconds += tv.tv_sec;

    while (tv.tv_sec < seconds)
	gettimeofday(&tv, NULL);

    return 0;
}

// JES drafts:

void *
_sbrk (int nbytes)
{
  errno = ENOMEM;
  return  (void *) -1;
}

/*
int _write(
   int fd,
   const void *buffer,
   unsigned int count
	   )
{
  errno = ENOSPC;
  return -1;
}
*/

int _write(int file, char *ptr, int len) {
  int todo;

#ifdef CONSOLE_UART
  for (todo = 0; todo < len; todo++) {
    ns16550_txchar (*ptr++);
  }
#endif
  return len;
}

int _close(
   int fd
	   )
{
  errno = EBADF;
  return -1;
}

long _lseek(
    int fd,
    long offset,
    int origin
)
{
  errno = EBADF;
  return -1;
}

int _read(
   int fd,
   void *buffer,
   unsigned int count
)
{
  errno = EBADF;
  return -1;
}

int _fstat(
   int fd,
   struct _stat *buffer
)
{
  errno = EBADF;
  return -1;
}

int _isatty(
  int fd
)
{
  errno = EBADF;
  return 0;
}

int _kill(
  int pid,
  int sig
)
{
  errno = EBADF;
  return -1;
}

int _getpid(
  int n
)
{
  return 1;
}



/*

 FROM FreeRTOS
*/
#define CAUSE_MACHINE_ECALL 0xb
#define SYS_write 64
#define SYS_exit 93
#define SYS_timer 1234


volatile uint64_t tohost __attribute__((aligned(64)));
volatile uint64_t fromhost __attribute__((aligned(64)));

uint64_t zeroExtend(long val)
{
	uint64_t ret = val;
	#if __riscv_xlen == 32
		ret = (0x00000000ffffffff & val);
	#endif
	return ret;
}

/* Relay syscall to host */
static uint64_t prvSyscallToHost(long which, long arg0, long arg1, long arg2)
{
	volatile uint64_t magic_mem[8] __attribute__((aligned(64)));
//    volatile uint64_t oldfromhost;
	magic_mem[0] = zeroExtend(which);
	magic_mem[1] = zeroExtend(arg0);
	magic_mem[2] = zeroExtend(arg1);
	magic_mem[3] = zeroExtend(arg2);
	__sync_synchronize();
    tohost = zeroExtend(magic_mem);
//    do
//    {
//        oldfromhost = fromhost;
//        fromhost = 0;
//    } while (oldfromhost == 0);
	return magic_mem[0];
}

/* Exit systemcall */
static void prvSyscallExit(long code)
{
	uint64_t zcode = zeroExtend(code);
	tohost = ((zcode << 1) | 1);
	for(;;) { }
}

/* Fires a syscall */
long syscall(long num, long arg0, long arg1, long arg2)
{
	register long a7 asm("a7") = num;
	register long a0 asm("a0") = arg0;
	register long a1 asm("a1") = arg1;
	register long a2 asm("a2") = arg2;
	asm volatile ("scall":"+r"(a0) : "r"(a1), "r"(a2), "r"(a7));
	return a0;
}

/* Trap handeler */
unsigned long ulSyscallTrap(long cause, long epc, long regs[32])
{
	long returnValue = 0;

	if (cause != CAUSE_MACHINE_ECALL) {
		prvSyscallExit(cause);
	} else if (regs[17] == SYS_exit) {
		prvSyscallExit(regs[10]);
	} else {
		returnValue = prvSyscallToHost(regs[17], regs[10], regs[11], regs[12]);
	}

	regs[10] = returnValue;
	return epc + 4;
}


#endif
