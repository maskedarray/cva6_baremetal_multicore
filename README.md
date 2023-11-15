# cva6_baremetal_multicore

## Build one test
1. In the root folder of repo do: source setup.sh
2. In the mohd_4cores folder do: make clean all EXTRA_FLAGS="-DRD_ON_RD". The general format is that any test can be written after -D. For example write on read can be written as: "-DWR_ON_RD"
3. It will generate mohd_4cores_.riscv file and mohd_4cores.dump file. .dump file is the assembly dump
