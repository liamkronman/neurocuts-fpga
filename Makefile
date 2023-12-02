
.PHONY: all
all: classbench neuro_cuts

neuro_cuts: neuro_cuts_tb.cc neuro_cuts.sv Makefile
	@echo "SV neuro_cuts.sv"
	@verilator --cc neuro_cuts.sv --public-flat-rw -Wno-UNOPTFLAT -Wno-UNOPT -Wno-CASEINCOMPLETE -Wno-WIDTH --exe neuro_cuts_tb.cc --build -j --top-module neuro_cuts
	@mv obj_dir/Vneuro_cuts neuro_cuts

classbench: classbench_parser.o
	@echo "classbench_parser"
	g++ -o classbench_parser classbench_parser.o

classbench.o: classbench_parser.cc
	g++ -c classbench_parser.cc

.PHONY: clean
clean:
	@echo "Cleaning Up!"
	@rm -rf obj_dir neuro_cuts
