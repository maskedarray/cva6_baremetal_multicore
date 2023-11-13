#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "pmu_test_func.c"


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
void test_cache2();
// Sometimes the UART skips over output.
// Gives the UART more time to finish output before filling up the UART Tx FIFO with more data.
void my_sleep() {
  uint32_t sleep = 100000;
  for (volatile uint32_t i=0; i<sleep; i++) {
    asm volatile ("fence");
    asm volatile ("addi x1, x1, 1");
    asm volatile ("fence");
  }  
}

void end_test(uint32_t mhartid){
  printf("Exiting my HartID: %0d,\n", mhartid);
}

#define write_32b(addr, val_)  (*(volatile uint32_t *)(long)(addr) = val_)

// *********************************************************************
// Main Function
// *********************************************************************
int main(int argc, char const *argv[]) {
  
  volatile uint32_t read_target_0;
  volatile uint32_t read_target_1;
  volatile uint32_t read_target_2;
  volatile uint32_t read_target_3;

  

  uint32_t mhartid;
  asm volatile (
    "csrr %0, 0xF14\n"
    : "=r" (mhartid)
  );

  uint32_t dspm_base_addr;
  uint32_t read_target;
  uint32_t error_count = 0;

  // uint32_t counter_val[]          = {0x1, 0x2, 0x3, 0x4};
  uint32_t event_sel_val[]  = {0x00001F, 0x00002F, 0x00003F, 0x00004F, 0x2F001F, 0x2F002F, 0x2F003F, 0x2F004F};
  // uint32_t event_info_val[]       = {0x10, 0x20, 0x30, 0x40};
  // uint32_t init_budget_val[]      = {0x100, 0x200, 0x300, 0x400};

  // counter_b_t counter_b[NUM_COUNTER];

  //                                                                                                                                                                                              

  // *******************************************************************
  // Core 0
  // *******************************************************************
  if (mhartid == 0) {   
    #ifdef FPGA_EMULATION
    uint32_t baud_rate = 9600;
    uint32_t test_freq = 100000000;
    #else
    set_flls();
    uint32_t baud_rate = 115200;
    uint32_t test_freq = 50000000;
    #endif

    uart_set_cfg(0,(test_freq/baud_rate)>>4);
    #ifdef RD_ON_RD
      printf("Read on read contention Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #elif WR_ON_WR
      printf("Write on write contention, Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #elif defined(RD_ONLY)
      printf("Only read in CUA, Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #elif WR_ONLY
      printf("Only write in CUA, Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #elif RD_ON_WR
      printf("Read on write contention, Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #elif WR_ON_RD
      printf("Write on read contention, Jump=%d Len=%d,\n", JUMP_CUA, LEN_NONCUA);
    #endif

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
    uint32_t event_sel[]    = {0x1F4F3F, 0x2F4F3F, 0x2F5F3F};
    write_32b_regs(EVENT_SEL_BASE_ADDR, 3, event_sel, COUNTER_BUNDLE_SIZE);
    uint32_t event_info[]    = {0x8001E0,0x8001E0,0x8001E0};
    write_32b_regs(EVENT_INFO_BASE_ADDR, 3, event_info, COUNTER_BUNDLE_SIZE);

    test_cache ();
    // test_cache2();
    end_test(mhartid);
    
    uart_wait_tx_done();

  // *******************************************************************
  // Core 1-3
  // *******************************************************************
  } else {
    // if (mhartid == 1) while(1){};
    // if (mhartid == 2) while(1){};
    // if (mhartid == 3) while(1){};

    while (1) {
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

  
  return 0;
}

void test_cache() {
  // For testing write, we need to be careful so as to not overwrite the program.
  // This is why the EVAL_LEN is restricted to 37 (4MB).
  #ifdef CUA_RD
    #define EVAL_LEN 37
  #elif defined (CUA_WR)
    #define EVAL_LEN 37
  #endif
  int eval_array[40] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360, 16384, 20480, 24576, 28672, 32768, 36864, 40960, 45056, 49152, 53248, 57344, 61440, 65536, 131072, 262144, 524288, 1048576};

  // uint32_t counter_rval[NUM_COUNTER];
  volatile uint64_t *array = (uint64_t*) 0x83000000;


  for(uint32_t a_len = 1; a_len < EVAL_LEN; a_len++) { 
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
          "sd   %0, 0(%1)\n"  // read addr_var data into read_var
          : "=r"(readvar1)
          : "r"(array - a_idx)
        );
      #endif
    }

    uint32_t counter_init[3], counter_final[3];
    // asm volatile("csrr %0, 0xb04" : "=r" (curr_miss) : );
    read_32b_regs(COUNTER_BASE_ADDR, 3, counter_init, COUNTER_BUNDLE_SIZE);
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
            "sd   %0, 0(%1)\n"  // read addr_var data into read_var
            : "=r"(readvar1)
            : "r"(array - a_idx)
          );
        #endif
      }
    }

    end_cycle = read_csr(cycle) - curr_cycle;
    read_32b_regs(COUNTER_BASE_ADDR, 3, counter_final, COUNTER_BUNDLE_SIZE);
    // asm volatile("csrr %0, 0xb04" : "=r" (end_miss) : );    

    // Size in Bytes.

    printf("%d,", (a_len2*8));


    // Load-store cycle count.

    // This line assumes that the repeat only has 4 instructions including the LD/ST.
    // This is compiler-dependent but I verified it when I wrote it.
    volatile uint64_t ld_sd_cc = (end_cycle)/(100*(a_len2/JUMP_CUA));
    printf("%d,", ld_sd_cc);


    // L1-D cache misses.
    // volatile uint64_t l1d_miss = end_miss - curr_miss;
    volatile uint64_t l1d_miss = (counter_final[2]-counter_init[2]);
    printf("%d,", (uint64_t)(((float)l1d_miss)/((float)end_cycle/250000.0))/*/((a_len2*8)/(JUMP_CUA*8))*/);



    printf("%d,", (counter_final[0]-counter_init[0])/(100*(a_len2/JUMP_CUA)));



    printf("%d\n", (counter_final[1]-counter_init[1])/(100*(a_len2/JUMP_CUA)));

  }
}


void test_cache2() {
  // For testing write, we need to be careful so as to not overwrite the program.
  // This is why the EVAL_LEN is restricted to 37 (4MB).
  #define EVAL_LEN 3

  int eval_array[3] = {1024, 40960, 524288};
  volatile uint64_t *array = (uint64_t*) 0x83000000;

  //CUA is reading
  for(uint32_t a_len = 0; a_len < EVAL_LEN; a_len++) {  
    uint64_t readvar1;
    uint64_t curr_cycle;
    uint64_t end_cycle;
    uint64_t a_len2;
    a_len2 = eval_array[a_len];

    for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
      asm volatile ( 
        "ld   %0, 0(%1)\n"  // read addr_var data into read_var
        : "=r"(readvar1)
        : "r"(array - a_idx)
      );
    }

    curr_cycle = read_csr(cycle);
    
    for (int a_repeat = 0; a_repeat < 100; a_repeat++){
      for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
        asm volatile ( 
          "ld   %0, 0(%1)\n"  // read addr_var data into read_var
          : "=r"(readvar1)
          : "r"(array - a_idx)
        );
      }
    }
    // Load-store cycle count.
    end_cycle = read_csr(cycle) - curr_cycle;
    volatile uint64_t ld_sd_cc = (end_cycle)/(100*(a_len2/JUMP_CUA));
    printf("%d,", ld_sd_cc);

  }
  printf("\n");
  my_sleep();
  // CUA is writing
  for(uint32_t a_len = 0; a_len < EVAL_LEN; a_len++) {  
    uint64_t readvar1;
    uint64_t curr_cycle;
    uint64_t end_cycle;
    uint64_t a_len2;
    a_len2 = eval_array[a_len];

    for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
      asm volatile ( 
        "sd   %0, 0(%1)\n"  // read addr_var data into read_var
        : "=r"(readvar1)
        : "r"(array - a_idx)
      );
    }

    curr_cycle = read_csr(cycle);
    
    for (int a_repeat = 0; a_repeat < 100; a_repeat++){
      for (int a_idx = 0; a_idx < a_len2; a_idx+=JUMP_CUA) {
        asm volatile ( 
          "sd   %0, 0(%1)\n"  // read addr_var data into read_var
          : "=r"(readvar1)
          : "r"(array - a_idx)
        );
      }
    }
    // Load-store cycle count.
    end_cycle = read_csr(cycle) - curr_cycle;
    volatile uint64_t ld_sd_cc = (end_cycle)/(100*(a_len2/JUMP_CUA));
    printf("%d,", ld_sd_cc);
  }
  printf("\n");
}

