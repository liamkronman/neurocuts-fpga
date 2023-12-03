
.PHONY: all
all: classbench neuro_cuts

neuro_cuts: neuro_cuts_tb.cc neuro_cuts.sv Makefile
	@echo "SV neuro_cuts.sv"
	@verilator --cc neuro_cuts.sv --public-flat-rw -Wno-UNOPTFLAT -Wno-UNOPT -Wno-CASEINCOMPLETE -Wno-WIDTH --exe neuro_cuts_tb.cc --build -j --top-module neuro_cuts
	@mv obj_dir/Vneuro_cuts neuro_cuts

class_bench:
	@echo "classbench_parser"
	@g++ -c classbench_parser.cc -o obj_dir/classbench_parser.o
	@g++ -o classbench_parser obj_dir/classbench_parser.o

.PHONY: clean
clean:
	@echo "Cleaning Up!"
	@rm -rf obj_dir neuro_cuts
