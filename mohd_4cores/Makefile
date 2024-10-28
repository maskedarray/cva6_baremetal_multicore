APP := mohd_4cores
XLEN:=64

EXTRA_FLAGS ?=
TEST_NAME ?=

include $(shell pwd)/../common_4cores.mk


program-fpga-llc:
	@pid=$$(pidof hw_server); \
	if [ -z "$$pid" ]; then \
		echo "hw_server is not running. Starting it..."; \
		nohup hw_server > hw_server.log 2>&1 & \
		echo "Started hw_server with PID: $$!"; \
	else \
		echo "hw_server is already running with PID: $$pid"; \
	fi
	xsdb -eval "connect -url TCP:localhost:3121;targets -set -nocase -filter {name =~ \"*xcvu9p*\"}; fpga -f ../../Bitstreams/Oct22_2024/alsaqr_xilinx2.bit"

# program-fpga:
# 	@pid=$$(pidof hw_server); \
# 	if [ -z "$$pid" ]; then \
# 		echo "hw_server is not running. Starting it..."; \
# 		nohup hw_server > hw_server.log 2>&1 & \
# 		echo "Started hw_server with PID: $$!"; \
# 	else \
# 		echo "hw_server is already running with PID: $$pid"; \
# 	fi
# 	xsdb -eval "connect -url TCP:localhost:3121;targets -set -nocase -filter {name =~ \"*xcvu9p*\"}; fpga -f ../../Bitstreams/Sep26_2024/core4-alsaqr_xilinx.bit"

# program-fpga-new-2core:
# 	@pid=$$(pidof hw_server); \
# 	if [ -z "$$pid" ]; then \
# 		echo "hw_server is not running. Starting it..."; \
# 		nohup hw_server > hw_server.log 2>&1 & \
# 		echo "Started hw_server with PID: $$!"; \
# 	else \
# 		echo "hw_server is already running with PID: $$pid"; \
# 	fi
# 	xsdb -eval "connect -url TCP:localhost:3121;targets -set -nocase -filter {name =~ \"*xcvu9p*\"}; fpga -f ../../Bitstreams/Sep28_2024/alsaqr_xilinx.bit"

# program-fpga-old:
# 	@pid=$$(pidof hw_server); \
# 	if [ -z "$$pid" ]; then \
# 		echo "hw_server is not running. Starting it..."; \
# 		nohup hw_server > hw_server.log 2>&1 & \
# 		echo "Started hw_server with PID: $$!"; \
# 	else \
# 		echo "hw_server is already running with PID: $$pid"; \
# 	fi
# 	xsdb -eval "connect -url TCP:localhost:3121;targets -set -nocase -filter {name =~ \"*xcvu9p*\"}; fpga -f ../../Bitstreams/Jan22/alsaqr_xilinx.bit"

run-openocd: 
	sudo /home/a26rahma/work/alsaqr/openocd_he_soc/riscv-openocd/src/openocd -f ../../Bitstreams/culsans-4core.cfg &

run-openocd-2core: 
	sudo /home/a26rahma/work/alsaqr/openocd_he_soc/riscv-openocd/src/openocd -f ../../Bitstreams/culsans-2core.cfg &

kill-openocd: 
	sudo kill -9 $$(pidof openocd) || true

rdrd4core:
	make clean all EXTRA_FLAGS=-DRD_ON_RD

rdwr4core:
	make clean all EXTRA_FLAGS=-DRD_ON_WR