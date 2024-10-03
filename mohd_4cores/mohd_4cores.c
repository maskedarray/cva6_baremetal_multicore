#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#ifdef USE_APMU
#include "pmu_test_func.c"
#include "pmu_defines.h"
#endif

// #define RD_ON_RD
// #define WR_ON_WR
// #define RD_ONLY
// #define WR_ONLY
// #define RD_ON_WR
// #define WR_ON_RD
#ifndef TESTS_AUTO
#define JUMP_CUA     8    // multiply by 8 for bytes
#define JUMP_NONCUA  8    // multiply by 8 for bytes
// #define LEN_NONCUA   32768 //256KB
#define LEN_NONCUA   524288   //4MB
#define START_NONCUA 0
#endif




#ifdef RD_ON_RD
  #define CUA_RD 
  #define INTF_RD
#elif defined(WR_ON_WR)
  #define CUA_WR
  #define INTF_WR 
#elif defined(RD_ONLY)
  #define CUA_RD
#elif defined(WR_ONLY)
  #define CUA_WR
#elif defined(RD_ON_WR)
  #define CUA_RD
  #define INTF_WR 
#elif defined(WR_ON_RD)
  #define CUA_WR
  #define INTF_RD 
#endif

void test_cache ();
void test_cache_small();


void end_test(uint32_t mhartid){
  printf("Exiting my HartID: %0d,\n", mhartid);
}

#define write_32b(addr, val_)  (*(volatile uint32_t *)(long)(addr) = val_)

uint32_t wait_barrier = 0;
// *********************************************************************
// Main Function
// *********************************************************************
int main(int mhartid, char const *argv[]) {


  // uint32_t mhartid;
  // asm volatile (
  //   "csrr %0, 0xF14\n"
  //   : "=r" (mhartid)
  // );

#ifdef USE_APMU
  uint32_t dspm_base_addr;
  uint32_t read_target;
  uint32_t error_count = 0;

  // uint32_t counter_val[]          = {0x1, 0x2, 0x3, 0x4};
  uint32_t event_sel_val[]  = {0x00001F, 0x00002F, 0x00003F, 0x00004F, 0x2F001F, 0x2F002F, 0x2F003F, 0x2F004F};
  // uint32_t event_info_val[]       = {0x10, 0x20, 0x30, 0x40};
  // uint32_t init_budget_val[]      = {0x100, 0x200, 0x300, 0x400};

  // counter_b_t counter_b[NUM_COUNTER];
#endif
                                                                                                                                                                                            

  // *******************************************************************
  // Core 0
  // *******************************************************************
  if (mhartid == 0) {   
    int baud_rate = 115200;
    int test_freq = 20000000;
    alsaqr_periph_fpga_padframe_periphs_cva6_uart_00_mux_set(1);
    uart_set_cfg(0,(test_freq/baud_rate)>>4);


    #ifdef RD_ON_RD
      printf("Read on read contention Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NONCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #elif WR_ON_WR
      printf("Write on write contention, Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NONCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #elif defined(RD_ONLY)
      printf("Only read in CUA, Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NONCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #elif WR_ONLY
      printf("Only write in CUA, Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NONCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #elif RD_ON_WR
      printf("Read on write contention, Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NONCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #elif WR_ON_RD
      printf("Write on read contention, Jump_CUA=%d JUMP_NONCUA=%d LEN_NCUA=%d,\n", JUMP_CUA, JUMP_NCUA, (LEN_NONCUA<131072)? (LEN_NONCUA/128) :  (LEN_NONCUA/131072));
    #endif
  
#ifdef USE_APMU
    // uint32_t program[] = {0x33,
    //                       0x137,
    //                       0x20010113, 
    //                       0x200213,
    //                       0x412023,
    //                       0x33,
    //                       0xfe000ee3};

    // uint32_t program_size = sizeof(program) / sizeof(program[0]);

    // uint32_t error_count = test_pmu_core_bubble_sort(ISPM_BASE_ADDR, DSPM_BASE_ADDR, STATUS_ADDR, 100, 2);
    // printf("CVA6-0! BubbleSort Errors: %d", error_count);
    // write_32b(STATUS_ADDR, 1);
    // printf("\n\n\n\n");

    // error_count = test_spm(ISPM_BASE_ADDR, program_size, program);
    // printf("CVA6-0! Errors: %d", error_count);


    // printf("Start PMU core.");
    // write_32b(STATUS_ADDR, 0);

    write_32b(0x50 + 0x10401000, 0xFFFFFF00);
    write_32b(0x54 + 0x10401000, 0xFFFF00FF);
    write_32b(0x58 + 0x10401000, 0xFF00FFFF);
    write_32b(0x5c + 0x10401000, 0x00FFFFFF);
    //                           // LLC IN  // LLC OUT
    uint32_t event_sel[]    = {MEM_RD_REQ_CORE_1 , LLC_RD_REQ_CORE_0 , MEM_RD_REQ_CORE_0};    // Read Request

    // this is for when cua is making reads
    // uint32_t event_sel[]  = {LLC_RD_REQ_CORE_0,   // llc read request by core 0
    //                          LLC_RD_RES_CORE_0,   // llc read response by core 0
    //                          MEM_RD_REQ_CORE_0,   // mem read request by core 0
    //                          MEM_RD_RES_CORE_0,
    //                          LLC_WR_REQ_CORE_0,   // llc write request by core 0
    //                          LLC_WR_RES_CORE_0,   // llc write response by core 0
    //                          MEM_WR_REQ_CORE_0,   // mem write request by core 0
    //                          MEM_WR_RES_CORE_0};  // mem write response by core 0
 

 
    // this doesn't change
    // uint32_t event_info[] = {0x000000,
    //                          ADD_RESP_LAT,
    //                          0x000000,
    //                          ADD_RESP_LAT,
    //                          0x000000,
    //                          ADD_RESP_LAT,
    //                          0x000000,
    //                          ADD_RESP_LAT,};
    write_32b_regs(EVENT_SEL_BASE_ADDR, 3, event_sel, COUNTER_BUNDLE_SIZE);
    uint32_t event_info[]    = {0,0,0};                                                       // Read Request

    write_32b_regs(EVENT_INFO_BASE_ADDR, 3, event_info, COUNTER_BUNDLE_SIZE);


      // printf("halting core %d", 2);
      // pmu_halt_core(ISPM_BASE_ADDR, PMC_STATUS_ADDR, 2, 0);
#endif

    // test_cache ();

    // printf("resuming core %d", 2);
    //   pmu_resume_core(ISPM_BASE_ADDR, PMC_STATUS_ADDR, 2, 0);
    // test_cache_small();
    test_cache();
    end_test(mhartid);
    
    uart_wait_tx_done();

  // *******************************************************************
  // Core 1-3
  // *******************************************************************
  } else {
    
    #ifdef CORE_SWEEP1
    if (mhartid == 1) while(1){};
    if (mhartid == 2) while(1){};
    #endif
    #ifdef CORE_SWEEP2
    if (mhartid == 1) while(1){};
    #endif
    #ifdef CORE_SWEEP3

    #endif

    while (1) {
      // printf("interfering core: %d\n", mhartid);
      asm volatile ("interfering_cores:");
      uint64_t readvar2;
      volatile uint64_t *array2 = (uint64_t*)(uint64_t)(0x83000000 + mhartid * 0x01000000);
      
      // 32'd1048576/2 = 0x0010_0000/2 elements.
      // Each array element is 64-bit, the array size is 0x0040_0000 = 4MB.
      // This will always exhaust the 2MB LLC.
      // Each core has 0x0100_0000 (16MB) memory.
      // It will iterate over 4MB array from top address to down
      for (int a_idx = START_NONCUA; a_idx < LEN_NONCUA; a_idx +=JUMP_NONCUA) {
        #ifdef INTF_RD
          asm volatile (
            "ld   %0, 0(%1)\n"  // read addr_var data into read_var
            : "=r"(readvar2)
            : "r"(array2 - a_idx)
          );
        #elif defined(INTF_WR)
          asm volatile (
            "sd   %1, 0(%0)\n"  // read addr_var data into read_var
            :: "r"(array2 - a_idx), "r"(readvar2)
          );
        #endif
      } 
    }
  }
  while(1){}
  
  return 0;
}


void test_cache_small() {
  // For testing write, we need to be careful so as to not overwrite the program.
  // This is why the EVAL_LEN is restricted to 37 (4MB).
  uint32_t eval_len = 3;

  int eval_array[3] = {1024, 40960, 524288};  // L1, L2, Main Memory
  uint64_t *array = (uint64_t*) 0x83000000;

  //CUA is reading or writing
  for(uint32_t a_len = 0; a_len < eval_len; a_len++) { // only test L2 and main memory 
    uint64_t readvar1;
    uint64_t curr_cycle;
    uint64_t end_cycle;
    uint64_t size_of_array;
    size_of_array = eval_array[a_len];
    uint32_t no_of_reps = 100;

    for (int a_idx = 0; a_idx < size_of_array; a_idx+=JUMP_CUA) {
      #ifdef CUA_RD
        asm volatile ( 
          "ld   %0, 0(%1)\n"  // read addr_var data into read_var
          : "=r"(readvar1)
          : "r"(array - a_idx)
        );
      #elif defined(CUA_WR)
        asm volatile (
            "sd   %1, 0(%0)\n"  // read addr_var data into read_var
            :: "r"(array - a_idx), "r"(readvar1)
          );
      #endif
    }

    #ifdef USE_APMU
    uint32_t counter_init[8], counter_final[8];
    read_32b_regs(COUNTER_BASE_ADDR, 8, counter_init, COUNTER_BUNDLE_SIZE);
    #endif
    
    curr_cycle = read_csr(cycle);

    #ifdef PRINT_TIME
    printf("Time at start: %d\n", curr_cycle);
    #endif    

    for (int a_repeat = 0; a_repeat < no_of_reps; a_repeat++){
      for (int a_idx = 0; a_idx < size_of_array; a_idx+=JUMP_CUA) {
        #ifdef CUA_RD
          asm volatile ( 
            "ld   %0, 0(%1)\n"  // read addr_var data into read_var
            : "=r"(readvar1)
            : "r"(array - a_idx)
          );
        #elif defined(CUA_WR)
          asm volatile (
              "sd   %1, 0(%0)\n"  // read addr_var data into read_var
              :: "r"(array - a_idx), "r"(readvar1)
            );
        #endif
      }
    }

    // Load-store cycle count.
    end_cycle = read_csr(cycle) - curr_cycle;
    volatile uint64_t ld_sd_cc = (end_cycle)/(no_of_reps * (size_of_array/JUMP_CUA));
    printf("%d", ld_sd_cc);

    #ifdef PRINT_TIME
    printf("Time at end: %d\n", end_cycle);
    printf("Difference: %d, Reps: %d, # of loads: %d\n", end_cycle-curr_cycle, no_of_reps, (size_of_array/JUMP_CUA));
    #endif
    
    #ifdef USE_APMU
    // latency counters
    read_32b_regs(COUNTER_BASE_ADDR, 8, counter_final, COUNTER_BUNDLE_SIZE);
    printf("%d,", (counter_final[1]-counter_init[1]) / (counter_final[0]-counter_init[0]) );
    printf("%d,", (counter_final[3]-counter_init[3]) / (counter_final[2]-counter_init[2]) );
    printf("%d,", (counter_final[5]-counter_init[5]) / (counter_final[4]-counter_init[4]) );
    printf("%d\n", (counter_final[7]-counter_init[7]) / (counter_final[6]-counter_init[6]) );
    #endif
  }
  
}


void test_cache() {
  // For testing write, we need to be careful so as to not overwrite the program.
  // This is why the EVAL_LEN is restricted to 37 (4MB).
  uint32_t eval_len = 38;
  #ifdef CUA_RD
    eval_len = 37;
  #elif defined (CUA_WR)
    eval_len = 37;
  #endif
  int eval_array[40] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360, 16384, 20480, 24576, 28672, 32768, 36864, 40960, 45056, 49152, 53248, 57344, 61440, 65536, 131072, 262144, 524288, 1048576};

  // uint32_t counter_rval[NUM_COUNTER];
  volatile uint64_t *array = (uint64_t*) 0x83000000;


  for(uint32_t a_len = 1; a_len < eval_len; a_len++) { 
    uint64_t readvar1, readvar2, readvar3, readvar4;
 
    uint64_t curr_cycle;
    uint64_t end_cycle;
    uint64_t curr_miss;
    uint64_t end_miss;
    uint64_t a_len2;
    a_len2 = eval_array[a_len];

    for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
      #ifdef CUA_RD
        asm volatile ( 
          "ld   %0, 0(%1)\n"  // read addr_var data into read_var
          : "=r"(readvar1)
          : "r"(array - a_idx)
        );
      #elif defined(CUA_WR)
        asm volatile (
            "sd   %1, 0(%0)\n"  // read addr_var data into read_var
            :: "r"(array - a_idx), "r"(readvar2)
          );
      #endif
    }

    #ifdef USE_APMU
    uint32_t counter_init[3], counter_final[3];
    // asm volatile("csrr %0, 0xb04" : "=r" (curr_miss) : );
    read_32b_regs(COUNTER_BASE_ADDR, 3, counter_init, COUNTER_BUNDLE_SIZE);
    #endif
    curr_cycle = read_csr(cycle);
    
    for (int a_repeat = 0; a_repeat < 100; a_repeat++){
      for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
        #ifdef CUA_RD
          asm volatile ( 
            "ld   %0, 0(%1)\n"  // read addr_var data into read_var
            : "=r"(readvar1)
            : "r"(array - a_idx)
          );
        #elif defined(CUA_WR)
          asm volatile (
            "sd   %1, 0(%0)\n"  // read addr_var data into read_var
            :: "r"(array - a_idx), "r"(readvar2)
          );
        #endif
      }
    }

    end_cycle = read_csr(cycle) - curr_cycle;
    #ifdef USE_APMU
    read_32b_regs(COUNTER_BASE_ADDR, 3, counter_final, COUNTER_BUNDLE_SIZE);
    #endif
    // asm volatile("csrr %0, 0xb04" : "=r" (end_miss) : );    

    // Size in Bytes.

    printf("%d,", (a_len2*8));


    // Load-store cycle count.

    // This line assumes that the repeat only has 4 instructions including the LD/ST.
    // This is compiler-dependent but I verified it when I wrote it.
    volatile uint64_t ld_sd_cc = (end_cycle)/(100*(a_len2/JUMP_CUA));
    printf("%d,", ld_sd_cc);

    //non cua miss percentage
    //kilo accesses per second
    #ifdef USE_APMU
    if (LEN_NONCUA == 524288){
      volatile uint64_t l1d_miss = (counter_final[0]-counter_init[0]);
      printf("%d,", (uint64_t)(((float)l1d_miss)/((float)end_cycle/250000.0))/*/((a_len2*8)/(JUMP_CUA*8))*/);
    } else printf("0,");

    printf("%d,", (counter_final[1]-counter_init[1])/(a_len2/JUMP_CUA));

    printf("%d\n", (counter_final[2]-counter_init[2])/(a_len2/JUMP_CUA));
    #endif

  }
}


