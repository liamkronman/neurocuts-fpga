
.PHONY: all
all: neuro_cuts

neuro_cuts: Makefile ./testing/neuro_cuts_tb.cc classifier.sv common.sv rule_match.sv tree_constants.sv
	@echo "SV classifier.sv"
	@verilator --cc tree_constants.sv common.sv rule_match.sv classifier.sv --public-flat-rw -Wno-UNOPTFLAT -Wno-UNOPT -Wno-CASEINCOMPLETE -Wno-WIDTH --exe ./testing/neuro_cuts_tb.cc --exe ./testing/classbench_parser.cc --build -j --top-module classifier --unroll-count 1024
	@mv obj_dir/Vclassifier classifier

.PHONY: clean
clean:
	@echo "Cleaning Up!"
	@rm -rf obj_dir classifier
