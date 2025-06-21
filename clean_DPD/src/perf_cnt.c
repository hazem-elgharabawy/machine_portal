// Functions prototypes
// for v > 8 & < 11
#include "../inc/perf_cnt.h"

 void enable_perf_cnt(void){
    uint32_t inh;
    /* enable cycle, instr and hpm3–18 */
    asm volatile("csrr %0, 0x320" : "=r"(inh));
    inh &= ~((1U<<0)|(1U<<2)|(((uint32_t)0xFFFF)<<3));
    asm volatile("csrw 0x320, %0" :: "r"(inh));
}
 void config_perf_cnt(void){
    /* configure events: hpm3–18 → events 0–15 */
    // asm volatile("csrw 0x323, %0" :: "r"(EVENT_CYCLES));
    // asm volatile("csrw 0x324, %0" :: "r"(EVENT_INSTR));
    asm volatile("csrw 0x325, %0" :: "r"(EVENT_LD_STALL));
    asm volatile("csrw 0x326, %0" :: "r"(EVENT_JR_STALL));
    asm volatile("csrw 0x327, %0" :: "r"(EVENT_IMISS));
    asm volatile("csrw 0x328, %0" :: "r"(EVENT_LD));
    asm volatile("csrw 0x329, %0" :: "r"(EVENT_ST));
    asm volatile("csrw 0x32A, %0" :: "r"(EVENT_JUMP));
    asm volatile("csrw 0x32B, %0" :: "r"(EVENT_BRANCH));
    asm volatile("csrw 0x32C, %0" :: "r"(EVENT_BR_TAKEN));
    asm volatile("csrw 0x32D, %0" :: "r"(EVENT_COMP_INSTR));
    // asm volatile("csrw 0x32E, %0" :: "r"(EVENT_PIPE_STALL));
    // asm volatile("csrw 0x32F, %0" :: "r"(EVENT_APU_TYPE));
    // asm volatile("csrw 0x330, %0" :: "r"(EVENT_APU_CONT));
    // asm volatile("csrw 0x331, %0" :: "r"(EVENT_APU_DEP));
    // asm volatile("csrw 0x332, %0" :: "r"(EVENT_APU_WB));
}


 void rst_start_perf_cnt(void){
    /* reset cycle, instr, hpm3–18 (low + high) */
    // asm volatile("csrw 0xB00, x0"); asm volatile("csrw 0xB80, x0");
    // asm volatile("csrw 0xB02, x0"); asm volatile("csrw 0xB82, x0");
    //Cycles + Instructions OFF Target: M used + OPT
    // asm volatile("csrw 0xB03, x0"); asm volatile("csrw 0xB83, x0");
    // asm volatile("csrw 0xB04, x0"); asm volatile("csrw 0xB84, x0");
    asm volatile("csrw 0xB05, x0"); asm volatile("csrw 0xB85, x0");
    asm volatile("csrw 0xB06, x0"); asm volatile("csrw 0xB86, x0");
    asm volatile("csrw 0xB07, x0"); asm volatile("csrw 0xB87, x0");
    asm volatile("csrw 0xB08, x0"); asm volatile("csrw 0xB88, x0");
    asm volatile("csrw 0xB09, x0"); asm volatile("csrw 0xB89, x0");
    asm volatile("csrw 0xB0A, x0"); asm volatile("csrw 0xB8A, x0");
    asm volatile("csrw 0xB0B, x0"); asm volatile("csrw 0xB8B, x0");
    asm volatile("csrw 0xB0C, x0"); asm volatile("csrw 0xB8C, x0");
    asm volatile("csrw 0xB0D, x0"); asm volatile("csrw 0xB8D, x0");
    //Cycle + Reset Target: OPT 
    asm volatile("csrw 0xB00, x0"); asm volatile("csrw 0xB80, x0");
    asm volatile("csrw 0xB02, x0"); asm volatile("csrw 0xB82, x0");
    //End
    // asm volatile("csrw 0xB0E, x0"); asm volatile("csrw 0xB8E, x0");
    // asm volatile("csrw 0xB0F, x0"); asm volatile("csrw 0xB8F, x0");
    // asm volatile("csrw 0xB10, x0"); asm volatile("csrw 0xB90, x0");
    // asm volatile("csrw 0xB11, x0"); asm volatile("csrw 0xB91, x0");
    // asm volatile("csrw 0xB12, x0"); asm volatile("csrw 0xB92, x0");
}

 void stop_perf_cnt(void){   
    /* disable cycle, instr and hpm3–18 */
    asm volatile("csrw 0x320, %0" : : "r"(0xffffffff));
}

 void display_perf_cnt(void){
    uint32_t val;
    /* read low halves and print */
    printf("Performance counters:\n");
    printf("Low halves:\n");
    printf("==========================================================================\n");
    asm volatile("csrr %0, 0xB00" : "=r"(val)); printf("MCYCLE       = %10u\n", val);
    asm volatile("csrr %0, 0xB02" : "=r"(val)); printf("MINSTR       = %10u\n", val);
    // asm volatile("csrr %0, 0xB03" : "=r"(val)); printf("CYCLES       = %10u\n", val);
    // asm volatile("csrr %0, 0xB04" : "=r"(val)); printf("INSTR        = %10u\n", val);
    asm volatile("csrr %0, 0xB05" : "=r"(val)); printf("LD_STALL     = %10u\n", val);
    asm volatile("csrr %0, 0xB06" : "=r"(val)); printf("JR_STALL     = %10u\n", val);
    asm volatile("csrr %0, 0xB07" : "=r"(val)); printf("IMISS        = %10u\n", val);
    asm volatile("csrr %0, 0xB08" : "=r"(val)); printf("LD           = %10u\n", val);
    asm volatile("csrr %0, 0xB09" : "=r"(val)); printf("ST           = %10u\n", val);
    asm volatile("csrr %0, 0xB0A" : "=r"(val)); printf("JUMP         = %10u\n", val);
    asm volatile("csrr %0, 0xB0B" : "=r"(val)); printf("BRANCH       = %10u\n", val);
    asm volatile("csrr %0, 0xB0C" : "=r"(val)); printf("BR_TAKEN     = %10u\n", val);
    asm volatile("csrr %0, 0xB0D" : "=r"(val)); printf("COMP_INSTR   = %10u\n", val);
    // asm volatile("csrr %0, 0xB0E" : "=r"(val)); printf("PIPE_STALL   = %10u\n", val);
    // asm volatile("csrr %0, 0xB0F" : "=r"(val)); printf("APU_TYPE     = %10u\n", val);
    // asm volatile("csrr %0, 0xB10" : "=r"(val)); printf("APU_CONT     = %10u\n", val);
    // asm volatile("csrr %0, 0xB11" : "=r"(val)); printf("APU_DEP      = %10u\n", val);
    // asm volatile("csrr %0, 0xB12" : "=r"(val)); printf("APU_WB       = %10u\n", val);
    printf("==========================================================================\n");
}
