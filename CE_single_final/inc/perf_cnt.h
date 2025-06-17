#ifndef PERF_CNT_H
#define PERF_CNT_H

#include <stdio.h>
#include <stdint.h>
//#include "pulp.h"

/* Pulpissimo custom CSRs */
#define CSR_PCMR 0x7E0
#define CSR_PCER 0x7E1
#define CSR_PCCR 0x7E2

/* Standard RISC-V performance CSRs */
#define CSR_MCOUNTINHIBIT 0x320
#define CSR_MCYCLE        0xB00
#define CSR_MINSTRET      0xB02
#define CSR_HPMCOUNTER3   0xB03
#define CSR_HPMCOUNTER4   0xB04
#define CSR_HPMCOUNTER5   0xB05
#define CSR_HPMCOUNTER6   0xB06
#define CSR_HPMCOUNTER7   0xB07
#define CSR_HPMCOUNTER8   0xB08
#define CSR_HPMCOUNTER9   0xB09
#define CSR_HPMCOUNTER10  0xB0A
#define CSR_HPMCOUNTER11  0xB0B
#define CSR_HPMCOUNTER12  0xB0C
#define CSR_HPMCOUNTER13  0xB0D
#define CSR_HPMCOUNTER14  0xB0E
#define CSR_HPMCOUNTER15  0xB0F
#define CSR_HPMCOUNTER16  0xB10
#define CSR_HPMCOUNTER17  0xB11
#define CSR_HPMCOUNTER18  0xB12

#define CSR_HPMCOUNTERH3  0xB83
#define CSR_HPMCOUNTERH4  0xB84
#define CSR_HPMCOUNTERH5  0xB85
#define CSR_HPMCOUNTERH6  0xB86
#define CSR_HPMCOUNTERH7  0xB87
#define CSR_HPMCOUNTERH8  0xB88
#define CSR_HPMCOUNTERH9  0xB89
#define CSR_HPMCOUNTERH10 0xB8A
#define CSR_HPMCOUNTERH11 0xB8B
#define CSR_HPMCOUNTERH12 0xB8C
#define CSR_HPMCOUNTERH13 0xB8D
#define CSR_HPMCOUNTERH14 0xB8E
#define CSR_HPMCOUNTERH15 0xB8F
#define CSR_HPMCOUNTERH16 0xB90
#define CSR_HPMCOUNTERH17 0xB91
#define CSR_HPMCOUNTERH18 0xB92

/* Event masks */
#define EVENT_CYCLES       (1U<<0)
#define EVENT_INSTR        (1U<<1)
#define EVENT_LD_STALL     (1U<<2)
#define EVENT_JR_STALL     (1U<<3)
#define EVENT_IMISS        (1U<<4)
#define EVENT_LD           (1U<<5)
#define EVENT_ST           (1U<<6)
#define EVENT_JUMP         (1U<<7)
#define EVENT_BRANCH       (1U<<8)
#define EVENT_BR_TAKEN     (1U<<9)
#define EVENT_COMP_INSTR   (1U<<10)
#define EVENT_PIPE_STALL   (1U<<11)
#define EVENT_APU_TYPE     (1U<<12)
#define EVENT_APU_CONT     (1U<<13)
#define EVENT_APU_DEP      (1U<<14)
#define EVENT_APU_WB       (1U<<15)

/* Pulpissimo PCER mask & PCMR flags */
#define SPR_PCER_ALL_EVENTS_MASK 0xFFFFFFFF
#define CSR_PCMR_ACTIVE          0x2

// Functions prototypes
// Configures the performance counters
void config_perf_cnt(void);
// Enables the performance counters
void enable_perf_cnt(void);
// Resets and starts the performance counters
void rst_start_perf_cnt(void);
// Stops the performance counters
void stop_perf_cnt(void);
// Displays the performance counters
void display_perf_cnt(void);

#endif
