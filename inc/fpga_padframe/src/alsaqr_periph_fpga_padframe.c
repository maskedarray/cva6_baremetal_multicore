
#include "alsaqr_periph_fpga_padframe.h"
#define  ALSAQR_PERIPH_FPGA_PADFRAME_PERIPHS_CONFIG0_BASE_ADDR ALSAQR_PERIPH_FPGA_PADFRAME_BASE_ADDRESS
#include "alsaqr_periph_fpga_padframe_periphs_regs.h"
#include "bitfield.h"

#define REG_WRITE32(addr, value) *((volatile uint32_t*) addr) = (uint32_t) value
#define REG_READ32(addr) *((volatile uint32_t*) addr)




void alsaqr_periph_fpga_padframe_periphs_cva6_uart_00_mux_set(alsaqr_periph_fpga_padframe_periphs_cva6_uart_00_mux_sel_t mux_sel) {
  const uint32_t address = ALSAQR_PERIPH_FPGA_PADFRAME_BASE_ADDRESS + ALSAQR_PERIPH_FPGA_PADFRAME_PERIPHS_CONFIG_CVA6_UART_00_MUX_SEL_REG_OFFSET;
  const uint32_t sel_size = 1;
  uint32_t field_mask = (1<<sel_size)-1;
  REG_WRITE32(address, mux_sel & field_mask);
}
