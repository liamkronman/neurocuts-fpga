#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vclassifier.h"
#include "classbench_parser.h"

using ClassifierPtr = std::unique_ptr<Vclassifier>;

void Init(ClassifierPtr& classifier) { }

bool test_classbench(ClassifierPtr& classifier, std::vector<ClassBenchLine> const& classbench)
{
    classifier->eval();
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