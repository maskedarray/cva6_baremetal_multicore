
#include "alsaqr_periph_padframe.h"
#define  ALSAQR_PERIPH_PADFRAME_PERIPHS_CONFIG0_BASE_ADDR ALSAQR_PERIPH_PADFRAME_BASE_ADDRESS
#include "alsaqr_periph_padframe_periphs_regs.h"
#include "bitfield.h"

#define REG_WRITE32(addr, value) *((volatile uint32_t*) (long)addr) = (uint32_t) value
#define REG_READ32(addr) *((volatile uint32_t*) (long) addr)

