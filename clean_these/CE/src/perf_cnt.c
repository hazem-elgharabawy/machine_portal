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
    asm volatile("csrw 0x323, %0" :: "r"(EVENT_CYCLES));
    asm volatile("csrw 0x324, %0" :: "r"(EVENT_INSTR));
    asm volatile("csrw 0x325, %0" :: "r"(EVENT_LD_STALL));
    asm volatile("csrw 0x326, %0" :: "r"(EVENT_JR_STALL));
    asm volatile("csrw 0x327, %0" :: "r"(EVENT_IMISS));
    asm volatile("csrw 0x328, %0" :: "r"(EVENT_LD));
    asm volatile("csrw 0x329, %0" :: "r"(EVENT_ST));
    asm volatile("csrw 0x32A, %0" :: "r"(EVENT_JUMP));
    asm volatile("csrw 0x32B, %0" :: "r"(EVENT_BRANCH));
    asm volatile("csrw 0x32C, %0" :: "r"(EVENT_BR_TAKEN));
    asm volatile("csrw 0x32D, %0" :: "r"(EVENT_COMP_INSTR));
    asm volatile("csrw 0x32E, %0" :: "r"(EVENT_PIPE_STALL));
    asm volatile("csrw 0x32F, %0" :: "r"(EVENT_APU_TYPE));
    asm volatile("csrw 0x330, %0" :: "r"(EVENT_APU_CONT));
    asm volatile("csrw 0x331, %0" :: "r"(EVENT_APU_DEP));
    asm volatile("csrw 0x332, %0" :: "r"(EVENT_APU_WB));
}


void rst_start_perf_cnt(void){
    /* reset cycle, instr, hpm3–18 (low + high) */
    asm volatile("csrw 0xC00, x0"); asm volatile("csrw 0xC80, x0");
    asm volatile("csrw 0xC02, x0"); asm volatile("csrw 0xC82, x0");
    asm volatile("csrw 0xC03, x0"); asm volatile("csrw 0xC83, x0");
    asm volatile("csrw 0xC04, x0"); asm volatile("csrw 0xC84, x0");
    asm volatile("csrw 0xC05, x0"); asm volatile("csrw 0xC85, x0");
    asm volatile("csrw 0xC06, x0"); asm volatile("csrw 0xC86, x0");
    asm volatile("csrw 0xC07, x0"); asm volatile("csrw 0xC87, x0");
    asm volatile("csrw 0xC08, x0"); asm volatile("csrw 0xC88, x0");
    asm volatile("csrw 0xC09, x0"); asm volatile("csrw 0xC89, x0");
    asm volatile("csrw 0xC0A, x0"); asm volatile("csrw 0xC8A, x0");
    asm volatile("csrw 0xC0B, x0"); asm volatile("csrw 0xC8B, x0");
    asm volatile("csrw 0xC0C, x0"); asm volatile("csrw 0xC8C, x0");
    asm volatile("csrw 0xC0D, x0"); asm volatile("csrw 0xC8D, x0");
    asm volatile("csrw 0xC0E, x0"); asm volatile("csrw 0xC8E, x0");
    asm volatile("csrw 0xC0F, x0"); asm volatile("csrw 0xC8F, x0");
    asm volatile("csrw 0xC10, x0"); asm volatile("csrw 0xC90, x0");
    asm volatile("csrw 0xC11, x0"); asm volatile("csrw 0xC91, x0");
    asm volatile("csrw 0xC12, x0"); asm volatile("csrw 0xC92, x0");
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
    asm volatile("csrr %0, 0xC00" : "=r"(val)); printf("MCYCLE       = %10u\n", val);
    asm volatile("csrr %0, 0xC02" : "=r"(val)); printf("MINSTR       = %10u\n", val);
    asm volatile("csrr %0, 0xC03" : "=r"(val)); printf("CYCLES       = %10u\n", val);
    asm volatile("csrr %0, 0xC04" : "=r"(val)); printf("INSTR        = %10u\n", val);
    asm volatile("csrr %0, 0xC05" : "=r"(val)); printf("LD_STALL     = %10u\n", val);
    asm volatile("csrr %0, 0xC06" : "=r"(val)); printf("JR_STALL     = %10u\n", val);
    asm volatile("csrr %0, 0xC07" : "=r"(val)); printf("IMISS        = %10u\n", val);
    asm volatile("csrr %0, 0xC08" : "=r"(val)); printf("LD           = %10u\n", val);
    asm volatile("csrr %0, 0xC09" : "=r"(val)); printf("ST           = %10u\n", val);
    asm volatile("csrr %0, 0xC0A" : "=r"(val)); printf("JUMP         = %10u\n", val);
    asm volatile("csrr %0, 0xC0B" : "=r"(val)); printf("BRANCH       = %10u\n", val);
    asm volatile("csrr %0, 0xC0C" : "=r"(val)); printf("BR_TAKEN     = %10u\n", val);
    asm volatile("csrr %0, 0xC0D" : "=r"(val)); printf("COMP_INSTR   = %10u\n", val);
    asm volatile("csrr %0, 0xC0E" : "=r"(val)); printf("PIPE_STALL   = %10u\n", val);
    asm volatile("csrr %0, 0xC0F" : "=r"(val)); printf("APU_TYPE     = %10u\n", val);
    asm volatile("csrr %0, 0xC10" : "=r"(val)); printf("APU_CONT     = %10u\n", val);
    asm volatile("csrr %0, 0xC11" : "=r"(val)); printf("APU_DEP      = %10u\n", val);
    asm volatile("csrr %0, 0xC12" : "=r"(val)); printf("APU_WB       = %10u\n", val);
    printf("==========================================================================\n");
}

