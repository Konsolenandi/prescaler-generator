#ifndef PS_CALC_H_
#define PS_CALC_H_

#include <stdint.h>

#define DEFAULT_TIM     tim0_ps
#define DEFAULT_TIM_NB  0
#define DEFAULT_CLKFREQ 8       // MHz
#define DEFAULT_INTV    1000    // us
#define DEFAULT_OUTF    "ps.inc"// Default name of generated file

#define MAXLEN_OUTF     64
#define MAXLEN_REPR     8
#define MAXLEN_TIMREG   UINT8_MAX
#define PRESC_DIV_TERM  0
#define PRESC_REPR_TERM "11111111"

typedef struct prescaler_s {
    unsigned int divisor;
    char representation[MAXLEN_REPR];
} prescaler_t;

#endif