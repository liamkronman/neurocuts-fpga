#include <verilated.h>
#include <time.h>
#include <memory.h>
#include <random>
#include <iostream>
#include "Vneuro_cuts.h"
#include "classbench_parser.h"

using NeuroCutsPtr = std::unique_ptr<Vneuro_cuts>;

int my_random() {
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_int_distribution<unsigned> distribution(1, 6);
    return distribution(rng);
}

void Init(NeuroCutsPtr& neuro_cuts) {
    neuro_cuts->a = 0;
}

bool test_classbench(NeuroCutsPtr& neuro_cuts, std::vector<Rule> const& rule)
{
    std::cout << "[out:" << int(neuro_cuts->b) << ']' << std::endl; 
    return true;
}

int main(int argc, char ** argv)
{
    Verilated::commandArgs(argc, argv);
    auto rule = parse_classbench_to_rule(std::string{argv[1]});
    auto neuro_cuts = std::make_unique<Vneuro_cuts>();
    Init(neuro_cuts);
    while (!Verilated::gotFinish()) {
        if (!test_classbench(neuro_cuts, rule)) {
            return -1;
        }
    }
}