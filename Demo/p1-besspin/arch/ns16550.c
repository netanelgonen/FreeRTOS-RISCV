
#include <stdint.h>
#include "ns16550.h"


#define DEFAULT_BAUDRATE  (9600)

/* Struct for 16550 register space */
struct __attribute__ ((aligned (4))) ns16550_pio
{
  /* 0x000 */
  union __attribute__ ((aligned (4))) {
    /* Receive buffer when read */
    const volatile uint8_t rbr;
    /* Transmit holding register when written */
    volatile uint8_t thr;
    /* Divisor latch (LSB) register when bit 7 of LCR is set */
    volatile uint8_t dll;
  };

  /* 0x004 */
  union __attribute__ ((aligned (4))) {
    /* Divisor latch (MSB) register when bit 7 of LCR is set */
    volatile uint8_t dlm;
    /* Interrupt enable register */
    volatile enum ier_t ier;
  };

  /* 0x008 */
  union __attribute__ ((aligned (4))) {
    /* Interrupt identification register */
    const volatile enum iir_t iir;
    /* FIFO control register when bit 7 of LCR is set */
    volatile enum fcr_t fcr;
  };

  /* 0x00c -- Line control register (LCR) */
  volatile enum lcr_t lcr __attribute__ ((aligned (4)));

  /* 0x010 -- Modem control register */
  volatile enum mcr_t mcr __attribute__ ((aligned (4)));

  /* 0x014 -- Line status register (LSR) */
  volatile enum lsr_t lsr __attribute__ ((aligned (4)));

  /* 0x018 -- Modem status register */
  volatile uint8_t msr __attribute__ ((aligned (4)));

  /* 0x01c -- Scratch register */
  volatile uint8_t scr __attribute__ ((aligned (4)));
};


static struct ns16550_pio * pio = (void*)NS16550_BASE;

#ifdef CONSOLE_UART
__attribute__ ((constructor))
int ns16550_init(void)
{
  uint32_t divisor;
  divisor = NS16550_CLOCK_RATE / (16 * DEFAULT_BAUDRATE);

  /* Disable all interrupts */
  pio->ier = 0;

  /* DLAB=1, Set Divisor Latch MSB and LSB registers */
  pio->lcr |= LCR_DLAB;
  pio->dll = divisor & 0xff;
  pio->dlm = (divisor >> 8) & 0xff;

  /* DLAB=0 Allow access to RBR, THR, IER, IIR registers*/
  pio->lcr &= ~LCR_DLAB;

  /* 8 bits/char, 1 stop bit */
  //pio->lcr = LCR_WLS8;
  /* 8 bits/char, 2 stop bits */
  pio->lcr = 7;

  /* Enable FIFOs */
  pio->fcr = FCR_FE;

  /* Drive RTSN (request to send) low */
  pio->mcr = MCR_RTS;

  return 0;
}
#endif


int ns16550_rxready(void)
{
  return ((pio->lsr & LSR_DR) != 0);
}


int ns16550_rxchar(void)
{
  while ((pio->lsr & LSR_DR) == 0)
    ;  // nothing

  return pio->rbr;
}


int ns16550_txchar(int c)
{
  while ((pio->lsr & LSR_THRE) == 0)
    ;  // nothing

  pio->thr = c;

  return c;
}

/* Wait for transmitter shift register/FIFO to empty */
void ns16550_flush(void)
{
  while ((pio->lsr & LSR_TEMT) == 0)
    ;  // nothing
}
