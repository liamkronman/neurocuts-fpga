
.PHONY: all
all: neuro_cuts

neuro_cuts: Makefile ./testing/neuro_cuts_tb.cc classifier.sv network_pkg.sv tree_constants.sv node.sv
	@echo "SV classifier.sv"
	@verilator --cc network_pkg.sv tree_constants.sv node.sv classifier.sv --public-flat-rw -Wno-UNOPTFLAT -Wno-UNOPT -Wno-CASEINCOMPLETE -Wno-WIDTH --exe ./testing/neuro_cuts_tb.cc --exe ./testing/classbench_parser.cc --build -j --top-module classifier --unroll-count 1024
	@mv obj_dir/Vclassifier classifier

.PHONY: clean
clean:
	@echo "Cleaning Up!"
	@rm -rf obj_dir classifier
