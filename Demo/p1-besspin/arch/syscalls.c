#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include "uart_16550.h"

/* To communicate with the debugger */
volatile uint64_t tohost __attribute__((aligned(64)));
volatile uint64_t fromhost __attribute__((aligned(64)));


#ifndef __linux__

void* _sbrk (int nbytes);
int _write(int file, char *ptr, int len);
int _close(int fd);
int _fstat(int fd, void *buffer);
long _lseek(int fd,long offset,int origin);
int _read(int fd,void *buffer,unsigned int count);
int _isatty(int fd);
int _kill(int pid,int sig);
int _getpid(int n);
unsigned long ulSyscallTrap(long cause, long epc, long regs[32] __attribute__((unused)));

void* _sbrk (int nbytes)
{
  (void)nbytes;
  errno = ENOMEM;
  return  (void *) -1;
}


int _write(int file, char *ptr, int len) 
{
  (void)file;
  int i;
  for( i=0; i<len; i++) {
    if (ptr[i] == '\n') {
      uart_txchar('\r');
    }
    uart_txchar(ptr[i]);
  }

  return len;
}

int _close(int fd)
{
  (void)fd;
  errno = EBADF;
  return -1;
}

long _lseek(int fd,long offset,int origin)
{
  (void)fd;
  (void)offset;
  (void)origin;
  errno = EBADF;
  return -1;
}

int _read(int fd,void *buffer,unsigned int count)
{
  (void)fd;
  (void)buffer;
  (void)count;
  errno = EBADF;
  return -1;
}

int _fstat(int fd, void *buffer)
{
  (void)fd;
  (void)buffer;
  errno = EBADF;
  return -1;
}

int _isatty(int fd)
{
  (void)fd;
  errno = EBADF;
  return 0;
}

int _kill(int pid,int sig)
{
  (void)pid;
  (void)sig;
  errno = EBADF;
  return -1;
}

int _getpid(int n)
{
  (void)n;
  return 1;
}
#endif /* #ifndef __linux__ */


/* Syscall handler placeholder */
/* A way to handle syscalls has not been implemented. 
TA1 teams can add their own handlers if they wish. */
unsigned long ulSyscallTrap(long cause, long epc, long regs[32] __attribute__((unused)))
{
	(void)cause;
	(void)epc;

	/* Infinite loop, just so we know we caused a syscall */
	for(;;) { }
	return epc + 4;
}
