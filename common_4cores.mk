ifdef nogui
	sim_flags = batch-mode=1 
endif

#ifdef simple_pad
#	rtl_flags = simple-padframe=1
#	sim_flags += simple-padframe=1 
#endif

current_dir = $(shell pwd)


ifdef CLUSTER_BIN
	cc-elf-y = -DCLUSTER_BIN_PATH=\"$(current_dir)/stimuli/cluster.bin\"  -DCLUSTER_BIN
endif

ifdef USE_HYPER
	cc-elf-y += -DUSE_HYPER
endif

utils_dir = ../inc/

inc_dirs = . drivers/inc string_lib/inc padframe/inc fpga_padframe/inc udma udma/cpi udma/i2c udma/spim udma/uart udma/sdio apb_timer gpio

src_dirs = . drivers/src string_lib/src udma/uart padframe/src fpga_padframe/src

SRC+=$(foreach d, $(src_dirs), $(wildcard $(utils_dir)$d/*.c))

INC+=$(foreach d, $(inc_dirs), -I$(utils_dir)$d)

ifneq ($(strip $(wildcard $(HW_HOME)/ip_list/fll_behav/driver)),)
	FLL_DRIVER=1
	INC += -I$(HW_HOME)/ip_list/fll_behav/driver/inc
	SRC += $(wildcard $(HW_HOME)/ip_list/fll_behav/driver/src/*.c)
endif

inc_dir := ../common/
inc_dir_culsans := ../common_culsans/

RISCV_PREFIX ?= riscv$(XLEN)-unknown-elf-
RISCV_GCC ?= $(RISCV_PREFIX)gcc

RISCV_OBJDUMP ?= $(RISCV_PREFIX)objdump -h --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.text.init --section=.data --section=.tohost --section=.sdata --section=.rodata --section=.sbss --section=.bss --section=.tdata --section=.tbss --section=.stack -t -s

RISCV_FLAGS     := -mcmodel=medany -static -std=gnu99 -DNUM_CORES=2 -Wno-int-to-pointer-cast -O2 -ggdb3 -ffast-math -fno-common -fno-builtin-printf $(INC)
RISCV_LINK_OPTS := -static -nostdlib -nostartfiles -lm -lgcc

################
## FPGA FLAGS ##
################

# When sw is compiled with fpga=1 uart baudrate is derived from a source clock of 40MHz - This should be used when you are NOT testing peripherals
ifdef fpga
	RISCV_FLAGS += -DFPGA_EMULATION
endif

# When sw is compiled with fpga_ethernet=1 uart baudrate is derived from a source clock of 50MHz - This should be used when you are testing peripherals
ifdef fpga_ethernet
	RISCV_FLAGS += -DFPGA_EMULATION
	RISCV_FLAGS += -DFPGA_ETHERNET
endif


ifdef FLL_DRIVER
	RISCV_FLAGS += -DFLL_DRIVER
endif


clean:
	rm -f *.riscv
	rm -f *.dump
	rm -f *.slm

build:
	$(RISCV_GCC) $(RISCV_FLAGS)  $(EXTRA_FLAGS) -T $(inc_dir)/test.ld $(RISCV_LINK_OPTS) $(cc-elf-y) $(inc_dir)/crt.S $(inc_dir)/syscalls.c -L $(inc_dir) $(APP).c $(SRC) -o $(APP)_$(TEST_NAME).riscv



dis:
	$(RISCV_OBJDUMP) $(APP)_$(TEST_NAME).riscv > $(APP)_$(TEST_NAME).dump

dump:
	$(SW_HOME)/elf_to_slm.py --binary=$(APP).riscv --vectors=hyperram0.slm
	cp hyperram*.slm  $(HW_HOME)/
	cp $(APP).riscv  $(HW_HOME)/
	echo $(APP).riscv | tee -a  $(HW_HOME)/regression.list

all: clean build dis # dump

testall:
	make clean
	make build dis TEST_NAME="RD_ONLY"  EXTRA_FLAGS="-D RD_ONLY"
	make build dis TEST_NAME="WR_ONLY"  EXTRA_FLAGS="-D WR_ONLY"
	make build dis TEST_NAME="RD_ON_RD" EXTRA_FLAGS="-D RD_ON_RD"
	make build dis TEST_NAME="WR_ON_WR" EXTRA_FLAGS="-D WR_ON_WR"
	make build dis TEST_NAME="RD_ON_WR" EXTRA_FLAGS="-D RD_ON_WR"
	make build dis TEST_NAME="WR_ON_RD" EXTRA_FLAGS="-D WR_ON_RD"

testall-sweep:
	make clean
	make build dis TEST_NAME="RD_ONLY"  EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D RD_ONLY"
	make build dis TEST_NAME="WR_ONLY"  EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D WR_ONLY"
	make build dis TEST_NAME="RD_ON_RD" EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D RD_ON_RD"
	make build dis TEST_NAME="WR_ON_WR" EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D WR_ON_WR"
	make build dis TEST_NAME="RD_ON_WR" EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D RD_ON_WR"
	make build dis TEST_NAME="WR_ON_RD" EXTRA_FLAGS="-DCORE_SWEEP1 -DCORES_SWEEP_TEST -D WR_ON_RD"


testall36:
	make clean
	make build TEST_NAME="RD_ONLY0" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ONLY0" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_RD0" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_WR0" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_WR0" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_RD0" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ONLY1" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ONLY1" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_RD1" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_WR1" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_WR1" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_RD1" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ONLY2" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ONLY2" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_RD2" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_WR2" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ON_WR2" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="WR_ON_RD2" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=32768"
	make build TEST_NAME="RD_ONLY3" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ONLY3" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_RD3" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_WR3" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_WR3" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_RD3" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=8 -D JUMP_NONCUA=8 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ONLY4" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ONLY4" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_RD4" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_WR4" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_WR4" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_RD4" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=16 -D JUMP_NONCUA=16 -D START_NONCUA=64 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ONLY5" EXTRA_FLAGS="-D  RD_ONLY -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ONLY5" EXTRA_FLAGS="-D  WR_ONLY -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_RD5" EXTRA_FLAGS="-D RD_ON_RD -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_WR5" EXTRA_FLAGS="-D WR_ON_WR -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="RD_ON_WR5" EXTRA_FLAGS="-D RD_ON_WR -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"
	make build TEST_NAME="WR_ON_RD5" EXTRA_FLAGS="-D WR_ON_RD -D TESTS_AUTO -D JUMP_CUA=2048 -D JUMP_NONCUA=2048 -D START_NONCUA=0 -D LEN_NONCUA=524288"


rtl: 
	 $(MAKE) -C  $(SW_HOME)/../hardware/ all 

sim:
	$(MAKE) -C  $(SW_HOME)/../hardware/ sim $(sim_flags) elf-bin=$(shell pwd)/$(APP).riscv
