#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vclassifier.h"
#include "classbench_parser.h"

using ClassifierPtr = std::unique_ptr<Vclassifier>;

void Init(ClassifierPtr& classifier) {
    classifier->reset = 1;
    classifier->clk = 0;
    classifier->eval();
    classifier->clk = 1;
    classifier->eval();
    classifier->reset = 0;
}

bool test_classbench(ClassifierPtr& classifier, std::vector<ClassBenchLine> const& classbench)
{
    classifier->clk ^= 1;
    if (classifier->finish) {
        std::cout << "finished" << std::endl;
    }
    classifier->eval();
    //if (classifier->clk) // positive edge
        //std::cout << (int)(classifier->number_of_rules) << std::endl;
    return true;
}

int main(int argc, char ** argv)
{
    Verilated::commandArgs(argc, argv);
    auto classbench = std::vector<ClassBenchLine>{}; //parse_classbench(std::string{argv[1]});
    auto classifier = std::make_unique<Vclassifier>();
    Init(classifier);
    while (!Verilated::gotFinish()) {
        if (!test_classbench(classifier, classbench)) {
            return -1;
        }
    }
}