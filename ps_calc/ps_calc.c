#include "ps_calc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <unistd.h>
extern char *optarg;
extern int opterr;
extern int optind;
extern int optopt; 

// Timer specific prescalers and corresponding binary representations
const prescaler_t tim0_ps[] = {{1, "001"}, {8, "010"}, {64, "011"}, {256, "100"}, {1024, "101"}, {PRESC_DIV_TERM, PRESC_REPR_TERM}};
const prescaler_t tim1_ps[] = {{1, "001"}, {8, "010"}, {64, "011"}, {256, "100"}, {1024, "101"}, {PRESC_DIV_TERM, PRESC_REPR_TERM}};
const prescaler_t tim2_ps[] = {{1, "001"}, {8, "010"}, {32, "011"}, {64, "100"}, {128, "101"}, {256, "110"}, {1024, "111"}, {PRESC_DIV_TERM, PRESC_REPR_TERM}};

const char help_string[] = {
"Prescaler Generator\n"
"Usage: ps_calc (-switches)\n"
"    switch - options regarding the argument.\n"
"\n"
"-t      [0,2]   : Timer to be used as sysclk\n"
"-c      [0,n]   : (MHz) Clock frequency of controller\n"
"-l      [0,n]   : (us) Interval after which interrupt is thrown\n"
"-o      \"str\"   : Output file\n"
};

int main (int argc, char *argv[]) {
    int option;
    bool showHelp = false;
    char outf_name[MAXLEN_OUTF] = DEFAULT_OUTF;
    const prescaler_t *timer = DEFAULT_TIM;
    unsigned int timer_nb = DEFAULT_TIM_NB;
    unsigned int f_clk = DEFAULT_CLKFREQ;
    unsigned int t_intv = DEFAULT_INTV;
    unsigned int incr = 0;
    if (argc < 2) {
        showHelp = true;
    }
    // Parse command line switches
    while((option = getopt(argc, argv, ":o:t:c:l:")) != -1){ //get option from the getopt() method
        switch(option){
          case 't': // Timer to be used as system clock
            printf("Timer: %s\n", optarg);
            timer_nb = strtol(optarg, NULL, 10);
            switch(timer_nb) {
                case 0:
                    timer = tim0_ps;
                    break;
                case 1:
                    timer = tim1_ps;
                    break;
                case 2:
                    timer = tim2_ps;
                    break;
                default:
                    timer = DEFAULT_TIM;
                    break;
            }
            break;
          case 'c': // Clock frequency in Hz
            printf("XCO-Frequency: %s MHz\n", optarg);
            f_clk = strtol(optarg, NULL, 10);
            break;
          case 'l': // Interval of desired interrupt
            printf("Sysclk interrupt after %s us\n", optarg);
            t_intv = strtol(optarg, NULL, 10);
            break;
          case 'o': 
            printf("Given File: %s\n", optarg);
            strncpy(outf_name, optarg, MAXLEN_OUTF);
            break;
          case ':':
            printf("Option needs a value\n");
            showHelp = true;
            break;
          case '?': // Option unknown
            printf("Unknown option: %c\n", optopt);
            showHelp = true;
            break;
          default:
            showHelp = true;
            break;
       }
    }
    if (showHelp) {
        printf(help_string);
    }
    incr = f_clk * t_intv;  // Increments at base frequency until interrupt must be thrown
    double incr_div = 0;    // Increments at divided frequency when interrupt would be thrown
    double dev_min = DBL_MAX;
    double dev_cur = DBL_MAX;
    unsigned int comp_at;
    unsigned int i_min;
    for (unsigned int i = 0 ; !(timer[i].divisor == PRESC_DIV_TERM || timer[i].representation == PRESC_REPR_TERM) ; i++) {
        incr_div = (double)incr / (double)timer[i].divisor;
        if (incr_div <= MAXLEN_TIMREG) {
            dev_cur = fabs(round(incr_div)-incr_div);
            if (dev_cur < dev_min) {
                // If deviation is smaller than smallest deviation found previously
                comp_at = incr_div;
                dev_min = dev_cur;
                i_min = i;
            }
        }
    }
    printf("Deviation: %f\n", dev_min);
    printf("Prescaler: %i\n", timer[i_min].divisor);
    printf("Representation: %s\n", timer[i_min].representation);
    printf("Compare at: %i\n", comp_at);

    FILE *fp = NULL;
    fp = fopen(outf_name, "a");
    if (fp == NULL) {
        printf("Couldn't create file %s\n", outf_name);
        return 1; // Error opening file
    }
    fprintf(fp, ".equ TIM%dDIV = 0b%s\n", timer_nb, timer[i_min].representation);
    fprintf(fp, ".equ TIM%dCOMPVAL = %u\n", timer_nb, comp_at);
    if (fclose(fp) == EOF) {
        printf("Error closing file %s\n", outf_name);
        return 1; // Error closing file
    }
    return 0;   // Success
}